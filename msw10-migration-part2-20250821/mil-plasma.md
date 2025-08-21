# Configuration and Tweaks to Plasma 6 for MIL's Desktop

## Overview

These are the steps I used to make my MIL's Plasma 6 desktop appear
similar to her existing Microsoft Windows 10 desktop.  She had
done some tweaking to its appearance herself over the years (which
admittedly surprised me).

I'd never used Plasma before beginning this project.  I wouldn't be
surprised if someone who knows Plasma might say to me, "Why'd you do
that?  Why didn't you just do ...".  If you're one of those people,
please tell me.  Just let me know how to do it better next time.


## Third Party KDE Plasma 6.4 themes

To give Plasma a closer appearance to my MIL's desktop, I used a
couple of third party themes and their icons.

To make the first theme available, I ran:
```
$ mkdir -p ~/github/yeyushengfan258
$ git -C ~/github/yeyushengfan258 clone \
        git@github.com:yeyushengfan258/We10XOS-kde.git
$ cd ~/github/yeyushengfan258/We10XOS-kde
$ ./install.sh
```

The other theme is available in the Pling store as
[Win10OS Aurorae-light Theme](https://www.pling.com/p/1464171).  Once
downloaded, I ran:
```
$ cd ~/.local/share/aurorae/themes
$ unzip ~/Downloads/Win10OS-light.zip
```

I logged out and relogged back in.  I'm not sure if it is necessary,
but wanted to just in case.

## System Settings

### Theming

I opened System Settings and selected `Color & Themes`.  Under
there:

* I clicked on `Get New...`, searched for `We10XOS-dark` and installed
it.

* While under `Global Themes`, I selected the theme `Breeze Twilight`
  and enabled `Appearance settings` and `Desktop and window layout`,
  then clicked `Apply`.

* Under `Colors`, I selected `Breeze Light` and clicked `Apply`.

* Under `Application Style`, I selected `Fusion` and clicked `Apply`.

* Under `Plasma Style`, I selected `We10XOS-dark` and clicked `Apply`.

* Under `Window Decorations`, I selected `Win10OS-light` and clicked
  `Apply`.

* Under `Icons`, I clicked on `Get New...`, searched for
  `Win10OS-cursors`, and installed it.  I closed that pane then
  selected `Win10OS Cursors`.

* Under `System Sounds`, I left as `Ocean`.

* Under `Splash Screen`, I left as `Breeze`.

* Under `Login Screen`, I selected `We10XOS-dark` and clicked `Apply`.

Done theming now!


###  Disabling Virtual Desktops

I thought MIL would find virtual desktops useless and confusing,
especially accidentally activating it when mousing over the upper
left corner, so I disabled it.

Under `Mouse & Touchpad > Screen Edges`, I clicked on the upper
left corner of the little monitor then selected `No Action`.  This
disabled the virtual desktop feature.

Close out the System Settings and return to the desktop.


## Desktop Background

MIL liked having Bing's Picture of the Day on her Win10 Desktop,
so I accommodated her preference.

On the Desktop background, right-click and select `Desktop and
Wallpaper` which opens `Desktop Folder Settings`.

Under `Desktop Folder Settings`, select `Wallpaper`, set `Wallpaper
type` as `Picture of the Day` then set `Provider` to `Bing`.

Select `Icons`, set `Arrangement` to `Top to Bottom` and select
`Lock in Place`.  Set `Icon size` to 40%.

Click `Apply`.


## Panel

At the bottom of the screen is a long bar that comprises Plasma's
"Panel".

The Panel has 5 areas, the Application Launcher (far left), Task
Manager (next to the Launcher), System Tray (right side), Digital
Clock (right of the Tray), and Peek at Desktop (far right).


### Task Manager

To make the Panel look more like Win10, at the bottom of the screen,
right-click on the Panel in the middle (Task Manager) away from
any Task icon and select `Show Panel Configuration`.  In `Panel
Settings`, set `Floating` to `Disabled`.  Click on `Exit Edit Mode`.


### System Tray

I felt MIL would like a more comprehensive weather widget, so I
installed Weather Widget Plus and disabled the native Weather
widget.

On the right is the System Tray.  Find its up arrow that looks
like a small roof.  Right-click on it and select `Add or Manage
Widgets...`.  On the left in the Widgets search box, click on `Get
New...` and enter a search for `Weather Widget Plus`.  Drag it to
the System Tray area.  Click on `Exit Edit Mode`.

Again, on the System Tray arrow, right-click and select `Configure
System Tray`.  Under `Entries`, disable `Weather Report`, set
`Weather Widget Plus` to `Always shown`, and `Networks` to
`Always hidden`.

In the Tray area, right-click on `Weather Widget Plus` then
select `Configure Weather Widget Plus`.  For `Location`,
add your own city (in her case, Champaign, IL with
https://openweathermap.org/city/4887158).  Remove all other cities.
Click `OK`.


### Application Launcher

Giving the apps she uses often familiar names might help with the
transition, so I changed the Plasma-specific names of those apps to
more generic ones and put their icons where she could conveniently
find them.

On the far left side on the Application Launcher, right-click
and select `Edit Application...`.

Search for `Dolphin`.  Change its name to `File Explorer`.  Click
`Save`.

Search for `Discover`.  Change its name to `App Store`.  Click
`Save`.

Search for `gscan2pdf`.  Change its name to `Scanner`.  Select
its icon.  Browse and select the file
`/usr/share/gscan2pdf/gscan2pdf.svg` Click `Save`.

Close the `Menu Editor`.

For the apps she'd regularly use, I added them to the Desktop,
the Task Manager, and to Favorites.

Left-click on the `Application Launcher`.  In its search box,
enter `Firefox`.  Right-click on Firefox.  In the contextual
menu, select `Add to Desktop`.  Right-click again and select
`Add to Favorites`.  Right-click again and select `Pin to
Task Manager`.

Repeat the steps in the above paragraph for `Chrome`, `LibreOffice
Writer`, and `gscan2pdf`.
