# Installation and Configuration of Fedora for MIL's Desktop


## Overview

My mother-in-law's Microsoft Windows 10 computer, a
[Dell OptiPlex 9020 SFF](https://dl.dell.com/topicspdf/optiplex-9020-desktop_owners-manual_en-us.pdf),
was too old to run Windows 11.  She didn't want to buy a new
computer if she could avoid it, so I offered to put Linux on it
along side Win10 to see if that was acceptable solution for her.

This file is a summary of the steps of how I installed and tweaked
[Fedora 42 KDE Plasma Edition](https://fedoraproject.org/kde/) for
that computer.

The file [mil-plasma.md](mil-plasma.md) describes the steps I used
to give KDE Plasma a similar look to her existing MS Windows 10
system.

Throughout this document, when I refer to `$USER`, this is the
name I used for her login as well as the system's name.


## Hardware Prep

To ensure her existing Microsoft Windows system would be completely
undisturbed, I installed Fedora to its own separate drive.  I also
did all the installation and configuration on an old desktop system
I had using an SSD SATA drive I had unused.  After I was happy with
my configuration and testing efforts, I transplanted the SSD into
her desktop system.

If you're using this file for a guide for your own Fedora
installation using an existing Win10 system, before you continue, I
would **strongly recommend*** disconnecting the cables to the existing
MS Windows drive.  That way no typos or mistakes during installation
could damage or destroy that drive.


## Fedora Installation

I installed Fedora to the new SSD as a new drive with default
partitioning and went through all the steps.

(If you have questions about installing Fedora, it's probably
better to seek out a YouTube video or a genAI for help rather
than trying to handle all the possible contingencies here.)

In the Welcome Center during setup, I enabled third party
repositories.

Before rebooting, if you disconnected the Win10 drive as I
recommended above, you may now shut the system down, reconnect it,
and power up.

After the system comes up and logging in, I used `Konsole` for a
CLI.  I used it to set the computer's name and also enabled the ssh
daemon by running:
```
$ sudo hostnamectl set-hostname $USER
$ sudo systemctl enable --now sshd
```

After the change, I rebooted again and re-logged in.

I continued installing some additional software packages from the
shell.  I could have used the Discover Software Center, but I prefer
the CLI:
```
$ sudo dnf install -y git gscan2pdf gparted btrbk btrfsmaintenance
$ sudo flatpak install -y com.google.Chrome
```

MIL wanted to be able to play some videos that required proprietary
video and audio codecs, so I ran:
```
$ sudo dnf install \
    https://mirrors.rpmfusion.org/free/fedora/rpmfusion-free-release-$(rpm -E %fedora).noarch.rpm \
    https://mirrors.rpmfusion.org/nonfree/fedora/rpmfusion-nonfree-release-$(rpm -E %fedora).noarch.rpm

$ sudo dnf config-manager setopt fedora-cisco-openh264.enabled=1
$ sudo dnf install libde265 x265 x265-libs
$ sudo dnf swap ffmpeg-free ffmpeg --allowerasing
$ sudo dnf upgrade \
        --setopt=install_weak_deps=False \
        --exclude=PackageKit-gstreamer-plugin \
        @multimedia
```


## System Settings

If you don't expect to do any remote support for your former Win10
user, you can skip this step.

Under `System Settings`, I selected `Remote Desktop`.  From its pane,
I enabled RDP Server, added `$USER` under `+ Add New` with a unique
password, set `Autostart on login`, and clicked `Apply`.


## System Multiboot

I wanted to offer the option of booting back into MS Windows in case
something wasn't working for her as expected and she needed to
temporarily run Win10.

In `/etc/default/grub`, I changed: `GRUB_TIMEOUT=5` to `GRUB_TIMEOUT=10`,
and added the line:
```
GRUB_DISABLE_OS_PROBER=false
```

Then ran:
```
$ sudo grub2-mkconfig -o /boot/grub2/grub.cfg
```

Then commented out the `GRUB_DISABLE_OS_PROBER` line.

Now when booting, grub should show you the `Windows Boot Manager`
entry to enable booting into MS Windows.


## Mount Win10 disk as read-only

I wanted to make all her files under Win10 available to her
under Linux.  However, I didn't want her to accidentally muck up
the Win10 disk, so I mounted it read-only.

Making the Win10 file system available turned out not that
important.  She never used it after I used it to copy all her
personal files to directories under her new Linux home area.

To enable Win10 mounting, run `lsblk --fs`.  Look for an `ntfs`
partition with the largest size and note its UUID for use below.

To prep mount points, I ran:
```
$ sudo mkdir /windows
$ mkdir /home/$USER/Win10
```

Then added to `/etc/fstab`:
```
UUID=<UUID value>    /windows           ntfs3  ro,uid=1000,gid=1000,umask=022,nofail 0 0
/windows/Users/User  /home/$USER/Win10  none   bind,ro,nofail 0 0
```

You may need to adjust `User` following `/windows/Users` to match
the user's name for the MS Windows system.

Then ran:
```
$ sudo systemctl daemon-reload
$ sudo mount /windows
```

I did an `ls /windows` to ensure it mounted correctly.  It did, so
I ran `sudo mount /home/$USER/Win10`.  Then ran `ls /home/$USER/Win10`
to ensure her Win10 files were visible now under that path.


## HP LaserJet Pro MFP M127fn

MIL has an HP LaserJet Pro MFP M127fn all-in-one printer.  To enable
all its features, I installed additional packages by running:
```
$ sudo dnf install hplip hplip-gui
$ hp-setup
$ scanimage -L
$ hp-scan -g
```

The scanner and printer now runs great!
