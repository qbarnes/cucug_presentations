#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Layout ──────────────────────────────────────────── */
#define GAME_W       800
#define GAME_H       500
#define UI_H          56
#define WIN_H        (GAME_H + UI_H)

/* ── Gameplay ─────────────────────────────────────────── */
#define PADDLE_W      12
#define PADDLE_H      80
#define PADDLE_X_L    20
#define PADDLE_X_R   (GAME_W - 20 - PADDLE_W)
#define PADDLE_SPEED   6.0
#define BALL_R         6
#define SPEED_INIT     5.0
#define SPEED_MAX     15.0
#define AI_SPEED       4.2

/* ── Mode-toggle widget ──────────────────────────────── */
#define TOGGLE_CX     70           /* horizontal centre of widget     */
#define TOGGLE_Y     (GAME_H + 22) /* top of buttons in window coords */
#define BTN_W         36
#define BTN_H         28

/* ── X11 globals ─────────────────────────────────────── */
static Display    *dpy;
static int         scr;
static Visual     *vis;
static Colormap    cmap;
static Window      win;
static GC          gc;
static Pixmap      buf;       /* back buffer                  */
static XftDraw    *xdraw;     /* Xft draw context on buf      */

static XftFont    *font_score; /* large score digits           */
static XftFont    *font_pause; /* "PAUSED" overlay             */
static XftFont    *font_btn;   /* toggle "1" / "2"             */
static XftFont    *font_ui;    /* controls hint + "PLAYERS"    */

static XftColor    xft_green;
static XftColor    xft_green_med;
static XftColor    xft_black;

static unsigned long px_green;
static unsigned long px_green_dim;
static unsigned long px_black;

static int     win_w = GAME_W, win_h = WIN_H; /* current window size */
static Picture src_pic, dst_pic;               /* XRender pictures    */

/* ── Game state ──────────────────────────────────────── */
static int    single_player = 1;
static int    paused        = 0;
static int    score_l       = 0;
static int    score_r       = 0;

static double left_y, right_y;
static double ball_x, ball_y, ball_dx, ball_dy, ball_speed;

static int key_w, key_s, key_up, key_dn;

/* ── Helpers ─────────────────────────────────────────── */

static unsigned long alloc_px(const char *name)
{
    XColor c;
    XParseColor(dpy, cmap, name, &c);
    XAllocColor(dpy, cmap, &c);
    return c.pixel;
}

static void alloc_xft(const char *name, XftColor *out)
{
    XColor xc;
    XRenderColor rc = {0};
    XParseColor(dpy, cmap, name, &xc);
    rc.red   = xc.red;
    rc.green = xc.green;
    rc.blue  = xc.blue;
    rc.alpha = 0xffff;
    XftColorAllocValue(dpy, vis, cmap, &rc, out);
}

static void fill_rect(int x, int y, int w, int h, unsigned long px)
{
    XSetForeground(dpy, gc, px);
    XFillRectangle(dpy, buf, gc, x, y, w, h);
}

/* Draw UTF-8 text horizontally centred around (cx, baseline_y). */
static void draw_cx(XftFont *f, XftColor *c, int cx, int by, const char *s)
{
    XGlyphInfo ext;
    XftTextExtentsUtf8(dpy, f, (const FcChar8 *)s, strlen(s), &ext);
    XftDrawStringUtf8(xdraw, c, f, cx - ext.xOff / 2, by,
                      (const FcChar8 *)s, strlen(s));
}

/* ── Game logic ──────────────────────────────────────── */

static void spawn_ball(void)
{
    double dir   = (rand() & 1) ? 1.0 : -1.0;
    double angle = ((rand() % 41) - 20) * M_PI / 180.0;
    ball_x = GAME_W / 2.0;
    ball_y = GAME_H / 2.0;
    ball_speed = SPEED_INIT;
    ball_dx = cos(angle) * SPEED_INIT * dir;
    ball_dy = sin(angle) * SPEED_INIT;
}

static void clamp_y(double *y)
{
    if (*y < 0)                *y = 0;
    if (*y > GAME_H - PADDLE_H) *y = GAME_H - PADDLE_H;
}

static void reflect(double py, int side)
{
    double rel   = (ball_y - (py + PADDLE_H / 2.0)) / (PADDLE_H / 2.0);
    double angle = rel * 65.0 * M_PI / 180.0;
    ball_speed = fmin(ball_speed + 0.4, SPEED_MAX);
    ball_dx = cos(angle) * ball_speed * side;
    ball_dy = sin(angle) * ball_speed;
}

static void update_paddles(void)
{
    if (single_player) {
        double ctr = left_y + PADDLE_H / 2.0;
        if      (ball_y < ctr - 4) left_y -= AI_SPEED;
        else if (ball_y > ctr + 4) left_y += AI_SPEED;
    } else {
        if (key_w) left_y -= PADDLE_SPEED;
        if (key_s) left_y += PADDLE_SPEED;
    }
    if (key_up) right_y -= PADDLE_SPEED;
    if (key_dn) right_y += PADDLE_SPEED;
    clamp_y(&left_y);
    clamp_y(&right_y);
}

static void update_ball(void)
{
    ball_x += ball_dx;
    ball_y += ball_dy;

    if (ball_y - BALL_R <= 0)       { ball_y = BALL_R;          ball_dy =  fabs(ball_dy); }
    if (ball_y + BALL_R >= GAME_H)  { ball_y = GAME_H - BALL_R; ball_dy = -fabs(ball_dy); }

    if (ball_dx < 0 &&
        ball_x - BALL_R <= PADDLE_X_L + PADDLE_W &&
        ball_x + BALL_R >= PADDLE_X_L &&
        ball_y + BALL_R >= left_y &&
        ball_y - BALL_R <= left_y + PADDLE_H) {
        ball_x = PADDLE_X_L + PADDLE_W + BALL_R;
        reflect(left_y, 1);
    }

    if (ball_dx > 0 &&
        ball_x + BALL_R >= PADDLE_X_R &&
        ball_x - BALL_R <= PADDLE_X_R + PADDLE_W &&
        ball_y + BALL_R >= right_y &&
        ball_y - BALL_R <= right_y + PADDLE_H) {
        ball_x = PADDLE_X_R - BALL_R;
        reflect(right_y, -1);
    }

    if (ball_x < -BALL_R)         { score_r++; spawn_ball(); }
    if (ball_x > GAME_W + BALL_R) { score_l++; spawn_ball(); }
}

/* ── Drawing ─────────────────────────────────────────── */

static void draw_center_line(void)
{
    XSetForeground(dpy, gc, px_green_dim);
    for (int y = 0; y < GAME_H; y += 22)
        XFillRectangle(dpy, buf, gc, GAME_W / 2 - 1, y, 2, 12);
}

static void draw_score(void)
{
    char s[16];
    XGlyphInfo ext;

    snprintf(s, sizeof s, "%d", score_l);
    XftTextExtentsUtf8(dpy, font_score, (const FcChar8 *)s, strlen(s), &ext);
    XftDrawStringUtf8(xdraw, &xft_green, font_score,
                      GAME_W / 2 - 28 - ext.xOff, 64,
                      (const FcChar8 *)s, strlen(s));

    snprintf(s, sizeof s, "%d", score_r);
    XftDrawStringUtf8(xdraw, &xft_green, font_score,
                      GAME_W / 2 + 28, 64,
                      (const FcChar8 *)s, strlen(s));
}

static void draw_toggle(void)
{
    int x1 = TOGGLE_CX - BTN_W;   /* left edge of "1" button */
    int x2 = TOGGLE_CX;           /* left edge of "2" button */
    int ty = TOGGLE_Y;

    draw_cx(font_ui, &xft_green_med, TOGGLE_CX, GAME_H + 16, "PLAYERS");

    /* button backgrounds */
    fill_rect(x1, ty, BTN_W, BTN_H, single_player  ? px_green : px_black);
    fill_rect(x2, ty, BTN_W, BTN_H, !single_player ? px_green : px_black);

    /* divider and outer border (drawn after fills so they're on top) */
    XSetForeground(dpy, gc, px_green);
    XFillRectangle(dpy, buf, gc, x2, ty, 1, BTN_H);
    XDrawRectangle(dpy, buf, gc, x1 - 1, ty - 1, BTN_W * 2 + 1, BTN_H + 1);

    /* labels */
    draw_cx(font_btn, single_player  ? &xft_black : &xft_green_med,
            x1 + BTN_W / 2, ty + BTN_H / 2 + 6, "1");
    draw_cx(font_btn, !single_player ? &xft_black : &xft_green_med,
            x2 + BTN_W / 2, ty + BTN_H / 2 + 6, "2");
}

static void draw_hint(void)
{
    const char *hint = single_player
        ? "COMPUTER: LEFT  |  UP/DN: RIGHT  |  SPACE: PAUSE  |  Q: QUIT"
        : "W/S: LEFT  |  UP/DN: RIGHT  |  SPACE: PAUSE  |  Q: QUIT";
    draw_cx(font_ui, &xft_green_med, GAME_W / 2 + 80, GAME_H + 38, hint);
}

static void draw_ui(void)
{
    fill_rect(0, GAME_H, GAME_W, UI_H, px_black);
    XSetForeground(dpy, gc, px_green_dim);
    XFillRectangle(dpy, buf, gc, 0, GAME_H, GAME_W, 1);
    draw_toggle();
    draw_hint();
}

static void draw_pause_overlay(void)
{
    int bw = 340, bh = 92;
    int bx = (GAME_W - bw) / 2, by = (GAME_H - bh) / 2;
    fill_rect(bx, by, bw, bh, px_black);
    XSetForeground(dpy, gc, px_green);
    XDrawRectangle(dpy, buf, gc, bx, by, bw - 1, bh - 1);
    draw_cx(font_pause, &xft_green,     GAME_W / 2, by + 50, "PAUSED");
    draw_cx(font_ui,    &xft_green_med, GAME_W / 2, by + 74, "PRESS SPACE TO RESUME");
}

/* Scale the fixed back-buffer onto the current window, preserving aspect
   ratio and painting black bars in any unused margin. */
static void blit_scaled(void)
{
    double sx    = (double)win_w / GAME_W;
    double sy    = (double)win_h / WIN_H;
    double scale = sx < sy ? sx : sy;
    int dw = (int)(GAME_W * scale);
    int dh = (int)(WIN_H  * scale);
    if (dw < 1) dw = 1;
    if (dh < 1) dh = 1;
    int dx = (win_w - dw) / 2;
    int dy = (win_h - dh) / 2;

    /* inverse scale transform on the source picture */
    XTransform xform = {{
        { XDoubleToFixed((double)GAME_W / dw), 0, 0 },
        { 0, XDoubleToFixed((double)WIN_H  / dh), 0 },
        { 0, 0, XDoubleToFixed(1.0) }
    }};
    XRenderSetPictureTransform(dpy, src_pic, &xform);

    /* black letterbox / pillarbox bars */
    XSetForeground(dpy, gc, px_black);
    if (dx > 0) {
        XFillRectangle(dpy, win, gc, 0,       0, dx,              win_h);
        XFillRectangle(dpy, win, gc, dx + dw, 0, win_w - dx - dw, win_h);
    }
    if (dy > 0) {
        XFillRectangle(dpy, win, gc, 0, 0,       win_w, dy);
        XFillRectangle(dpy, win, gc, 0, dy + dh, win_w, win_h - dy - dh);
    }

    XRenderComposite(dpy, PictOpSrc, src_pic, None, dst_pic,
                     0, 0, 0, 0, dx, dy, dw, dh);
    XFlush(dpy);
}

static void draw_frame(void)
{
    fill_rect(0, 0, GAME_W, GAME_H, px_black);
    draw_center_line();
    draw_score();
    XSetForeground(dpy, gc, px_green);
    XFillRectangle(dpy, buf, gc, PADDLE_X_L, (int)left_y,  PADDLE_W, PADDLE_H);
    XFillRectangle(dpy, buf, gc, PADDLE_X_R, (int)right_y, PADDLE_W, PADDLE_H);
    XFillArc(dpy, buf, gc,
             (int)ball_x - BALL_R, (int)ball_y - BALL_R,
             BALL_R * 2, BALL_R * 2, 0, 360 * 64);
    if (paused) draw_pause_overlay();
    draw_ui();
    blit_scaled();
}

/* ── Button hit-test ─────────────────────────────────── */

static void set_mode(int single)
{
    single_player = single;
    score_l = score_r = 0;
    spawn_ball();
}

static void handle_click(int x, int y)
{
    int x1 = TOGGLE_CX - BTN_W, x2 = TOGGLE_CX;
    if (y < TOGGLE_Y || y >= TOGGLE_Y + BTN_H) return;
    if      (x >= x1 && x < x2)            set_mode(1);
    else if (x >= x2 && x < x2 + BTN_W)    set_mode(0);
}

/* ── Main ────────────────────────────────────────────── */

int main(void)
{
    srand((unsigned)time(NULL));

    dpy  = XOpenDisplay(NULL);
    if (!dpy) { fputs("Cannot open X display\n", stderr); return 1; }
    scr  = DefaultScreen(dpy);
    vis  = DefaultVisual(dpy, scr);
    cmap = DefaultColormap(dpy, scr);

    px_green     = alloc_px("#00ff41");
    px_green_dim = alloc_px("#004d15");
    px_black     = alloc_px("#000000");

    XSetWindowAttributes swa = {0};
    swa.background_pixel = px_black;
    swa.event_mask = KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ExposureMask |
                     StructureNotifyMask;
    win = XCreateWindow(dpy, RootWindow(dpy, scr),
                        100, 100, GAME_W, WIN_H, 0,
                        CopyFromParent, InputOutput, vis,
                        CWBackPixel | CWEventMask, &swa);
    XStoreName(dpy, win, "PONG");

    Atom wm_del = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wm_del, 1);

    gc    = XCreateGC(dpy, win, 0, NULL);
    buf   = XCreatePixmap(dpy, win, GAME_W, WIN_H, DefaultDepth(dpy, scr));
    xdraw = XftDrawCreate(dpy, buf, vis, cmap);

    XRenderPictFormat      *rfmt  = XRenderFindVisualFormat(dpy, vis);
    XRenderPictureAttributes pattr = {0};
    src_pic = XRenderCreatePicture(dpy, buf, rfmt, 0, &pattr);
    dst_pic = XRenderCreatePicture(dpy, win, rfmt, 0, &pattr);
    XRenderSetPictureFilter(dpy, src_pic, FilterBilinear, NULL, 0);

    alloc_xft("#00ff41", &xft_green);
    alloc_xft("#007722", &xft_green_med);
    alloc_xft("#000000", &xft_black);

    font_score = XftFontOpenName(dpy, scr, "monospace:bold:pixelsize=48");
    font_pause = XftFontOpenName(dpy, scr, "monospace:bold:pixelsize=34");
    font_btn   = XftFontOpenName(dpy, scr, "monospace:bold:pixelsize=16");
    font_ui    = XftFontOpenName(dpy, scr, "monospace:pixelsize=11");
    if (!font_score || !font_pause || !font_btn || !font_ui) {
        fputs("Could not load fonts\n", stderr);
        return 1;
    }

    XMapWindow(dpy, win);

    left_y  = (GAME_H - PADDLE_H) / 2.0;
    right_y = (GAME_H - PADDLE_H) / 2.0;
    spawn_ball();

    struct timespec frame = { 0, 16666667L }; /* ~60 Hz */

    for (int running = 1; running; ) {
        while (XPending(dpy)) {
            XEvent ev;
            XNextEvent(dpy, &ev);
            switch (ev.type) {
            case KeyPress: {
                KeySym k = XLookupKeysym(&ev.xkey, 0);
                if (k == XK_w || k == XK_W)      key_w  = 1;
                if (k == XK_s || k == XK_S)      key_s  = 1;
                if (k == XK_Up)                  key_up = 1;
                if (k == XK_Down)                key_dn = 1;
                if (k == XK_space)               paused ^= 1;
                if (k == XK_q || k == XK_Escape) running = 0;
                break;
            }
            case KeyRelease: {
                KeySym k = XLookupKeysym(&ev.xkey, 0);
                if (k == XK_w || k == XK_W) key_w  = 0;
                if (k == XK_s || k == XK_S) key_s  = 0;
                if (k == XK_Up)             key_up = 0;
                if (k == XK_Down)           key_dn = 0;
                break;
            }
            case ButtonPress:
                handle_click(ev.xbutton.x, ev.xbutton.y);
                break;
            case ConfigureNotify:
                win_w = ev.xconfigure.width;
                win_h = ev.xconfigure.height;
                break;
            case ClientMessage:
                if ((Atom)ev.xclient.data.l[0] == wm_del)
                    running = 0;
                break;
            }
        }

        if (!paused) {
            update_paddles();
            update_ball();
        }
        draw_frame();
        nanosleep(&frame, NULL);
    }

    XftFontClose(dpy, font_score);
    XftFontClose(dpy, font_pause);
    XftFontClose(dpy, font_btn);
    XftFontClose(dpy, font_ui);
    XftColorFree(dpy, vis, cmap, &xft_green);
    XftColorFree(dpy, vis, cmap, &xft_green_med);
    XftColorFree(dpy, vis, cmap, &xft_black);
    XRenderFreePicture(dpy, src_pic);
    XRenderFreePicture(dpy, dst_pic);
    XftDrawDestroy(xdraw);
    XFreePixmap(dpy, buf);
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    return 0;
}
