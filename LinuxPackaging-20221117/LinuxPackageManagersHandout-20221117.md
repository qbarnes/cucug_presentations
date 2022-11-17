## Common package use cases for Apt and DNF systems

### To see all installed packages:

For Apt systems, run:

```
$ dpkg -l
```

For DNF systems, run:

```
$ rpm -qa
```

### To update all the packages on your system:

For Apt systems, run:

```
$ sudo apt update
$ sudo apt upgrade
```

For DNF systems, run:

```
$ sudo dnf update
```

### To install a given package (e.g. `git`):

For Apt systems, run:

```
$ sudo apt update
$ sudo apt install git
```

For DNF systems, run:

```
$ sudo dnf install git
```

### To remove a given package and dependencies (e.g. `git`):

For Apt systems, run:

```
$ sudo apt autoremove git
```

For DNF systems, run:

```
$ sudo dnf remove git
```

### To find an installed package that provides a file (e.g. `/usr/bin/ls`):

For Apt systems, run:

```
$ dpkg -S /usr/bin/ls  # Oops!  Doesn't work!
$ dpkg -S /bin/ls
```

For DNF systems, run:

```
$ rpm -qf /usr/bin/ls
```

### Install package that provides file (e.g. `/usr/bin/convert`):

For Apt systems, run:

```
$ sudo apt install apt-file
$ sudo apt-file update
$ apt-file search -F /usr/bin/convert
$ sudo apt install graphicsmagick-imagemagick-compat
```

For DNF systems, run:

```
$ sudo dnf install /usr/bin/convert
```


### List files provided by installed package `git`:

For Apt systems, run:

```
$ dpkg -L git
```

For DNF systems, run:

```
$ rpm -ql git
```

### Find any available package name by pattern:

For Apt systems, run:

```
$ sudo apt update
$ apt list '*git*'
```

For DNF systems, run:

```
$ dnf repoquery '*git*'
```

### See package install history:

For Apt systems, run:

```
$ grep " install " /var/log/apt/history.log
$ zgrep " install " /var/log/apt/history.log.*.gz
```

For DNF systems, run:

```
$ dnf history
```

(For some more examples, google "archwiki pacman rosetta".)
