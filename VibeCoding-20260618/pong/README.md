This is my first attempt at vibe coding an entire app from scratch
using [Claude Code](https://claude.com/product/claude-code).
My full conversation with Claude Code can be read
[here](claudecodeconv.txt).

I chose the
[Sonnet 4.6](https://www.anthropic.com/news/claude-sonnet-4-6)
model to see what Claude Code's lowest available model was capable
of (its default).  And admittedly, to minimize the amount of my
monthly Claude allocation it would use.

To get started, my first prompt was, **Make me a Pong-style game
in a single HTML file, two paddles, score display, retro
green-on-black look.**

In 55 seconds (0m55s), Claude Code (CC) returned this
[pong.html](htmltry1/pong.html) written in HTML.  It was a
good first attempt.

But I'd like the computer to play one of the players, so my next
prompt was, **Can you adjust the game so that the computer plays
the left paddle?**

In 0m16s, CC returned this [pong.html](htmltry2/pong.html).
It worked great!

But now I wanted to be able to select between the two possibilities,
so I asked CC, **Can you place a button on the screen that allows the
user to select between single player (using computer control for one
paddle) and two player?**

In 0m39s, CC returned this [pong.html](htmltry3/pong.html).
It worked, but I felt the button interface was confusing.

To see if I could correct the interface, I next asked, **The
interface is confusing.  It states "1 player" when it's in two
player mode and vice versa.  Could you change it so it uses a mode
toggle button that has "1" on one side and "2" on the other with
"Players" above it?**

In 0m29s, CC returned this [pong.html](htmltry4/pong.html).
I felt that version was working now as I wanted.

Time now to go for another implementation and platform, so I asked,
**Can you convert pong.html so that it is now in C and port it to
X11 so that it can be compiled and run on Linux?**

In 12m49s, CC created me [pong.c](ctry1/pong.c) and
[Makefile](ctry1/Makefile), but the Makefile originally had a
bug in it which CC detected when testing and corrected
automatically.

While trying out the binary it created for me, I noticed that if
I resized its window, the window resized but the game stayed in
its original geometry, so I asked, **When I resize the X11 window
opening by the pong binary, the contents of the window (the game)
does not resize with the change.  Can you update the game so that it
scales with the resize?**

In 2m51s, CC made the game resizable updating to this
[pong.c](ctry2/pong.c) version with an updated
[Makefile](ctry2/Makefile).  And it worked great!  All done!

In true "[vibe coding](https://en.wikipedia.org/wiki/Vibe_coding)"
form, at no time did I even look (let alone change!) any of the
HTML or C source code that Claude Code generated while doing the
above steps.  Afterward, I looked at it and it seemed on par with
what a junior to intermediate human software developer might write
for both style and design.

During this run, I was concerned about chewing up gobs of my Claude
time.  I'm only on the $20/month tier.  Once I finished all the
above, I had only used about 1/3 of my 4 hour time slot, and my
monthly allotment had only been dinged by about 2%-3%!
