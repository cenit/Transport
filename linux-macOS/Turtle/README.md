# Turtle

Version 1.03

```bash
by:    Urs C. Rohrer
       PSI
       Area West
       CH-5232 Villigen-PSI
       Switzerland

email: Urs.Rohrer@psi.ch
phone: +41 56 310 40 35
```

## Turtle, a Computer Program for Designing Charged Particle Beam Transport Systems

This version of the computer program Decay Turtle is based on a CERN version
by K.L. Brown & Ch. Iselin (see: CERN 74-2) and is written in Fortran-77. Only
the graphical X-windows interface is written in C. Some sample input files are
included in this package (.dat). If you choose one, it has to be copied to
FOR001.DAT. In order to run Turtle you have to start ttmenu.

## X Command Shell

If you start ttmenu, then a X-window menu containing buttons for all
available commands is popping up. Ttmenu runs only if the wish-shell
(Tcl/Tk) is present in the system (e.g. in /usr/bin/).

## KDE support

In order to be able to start Turtle via an icon in the taskbar of
the kde desktop, turtle.xpm and kdestart have been included. turtle.xpm has
to be copied into the kde icons directory [e.g. /opt/kde/share/icons or
/usr/share/icons] and a kfm link to kdestart has to be established by
dragging and dropping kdestart onto the taskbar. Do not forget to enable
the necessary permissions when you are running as a non-superuser.
kdestart has also to be edited in order to include the directory name of
your installation.

## Hints

1. If you do not like to use emacs as the editor started from ttmenu, then you have to edit ttmenu and replace all occurrences of emacs by the name of your preferred editor (e.g. gedit).
2. The Turtle input files have to have only a LF (line feed) as line delimitator. If they are imported from a Windows or VAX environment, then the line delimiters are CR+LF and Turtle will not work.

## Prerequisites

In order to be able to build Turtle, you need the `CERNLIB` and `Curses` package installed.
On Ubuntu it is as easy as

```bash
sudo apt-get install cernlib libncurses5-dev libgeant321-2-dev libhepmc-dev
```

On macOS it is a little bit more complicated:

1. First of all install homebrew

2. Install these packages:

```bash
brew install gcc imake openmotif lapack openblas
```

3. Prepare the CERNLIB folder (requires super-user privileges, move to another folder only if really necessary - untested!)

```bash
cd /opt/
sudo mkdir cern
sudo chown $(whoami):$(id -g -n $(whoami)) cern
cd cern/
mkdir cernlib

# export CERN=/opt/cern
# export CERN_ROOT=/opt/cern
# export CERNLIB=$CERN/2006/lib
# export CERNBIN=$CERN/2006/bin
# export PATH=$PATH:$CERNBIN
```

4. Run the `build_cernlib_v2006_macOS.sh` script
