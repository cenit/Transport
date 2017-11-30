			     Turtle for Linux.
			     =================

by:    Urs C. Rohrer
       PSI
       Area West
       CH-5232 Villigen-PSI
       Switzerland

email:  Urs.Rohrer@psi.ch
phone: +41 56 310 40 35

-------------------------------------------------------------------------------

   This is Version 1.03 for Linux developped for systems with kernel 2.2.x
    (tested with newest systems from Redhat, SuSE, Mandrake and Caldera).

-------------------------------------------------------------------------------

         Turtle, a Computer Program for Designing Charged
                  Particle Beam Transport Systems.
         ------------------------------------------------


  This version of the computer program Decay Turtle is based on a CERN version
by K.L. Brown & Ch. Iselin (see: CERN 74-2) and is written in Fortran-77. Only
the graphical X-windows interface is written in C. This distribution has been
compiled with GNU compilers (g77 and gcc). Some sample input files are
included in this package (*.dat). If you choose one, it has to be copied to
FOR001.DAT. In order to run Turtle you have to start ttmenu.

X Command Shell:
================
If you start ttmenu, then a X-window menu containing buttons for all
available commands is poping up. Ttmenu runs only if the wish-shell
(Tcl/Tk) is present in the system (e.g. in /usr/bin/).
 
KDE support:
============
In order to be able to start Turtle via an icon in the taskbar of
the kde desktop, turtle.xpm and kdestart have been included. turtle.xpm has
to be copied into the kde icons directory [e.g. /opt/kde/share/icons or
/usr/share/icons] and a kfm link to kdestart has to be established by
dragging and dropping kdestart onto the taskbar. Do not forget to enable
the necessary permissions when you are running as a non-superuser.
kdestart has also to be edited in order to include the directory name of
your installation.

Current Directory:
==================
In new versions of Linux the current directory (./) is not anymore
included in the path declaration. Therefore, all programs which contain
references to other programs of the turtle directory were modified to
include a reference to the current directory. In case a reference has 
been forgotten to be changed, you may fix it by including ./: in the path
declaration e.g. of the file /etc/profile.

Libraries (libc6):
==================
Shared libraries /lib/libc.so.6 and /lib/libm.so.6 are required for all
executables. Additionally /usr/X11R6/lib/libX11.so.6 is required for
the executable displaw.

Hints:
======
1) If you do not like to use emacs as the editor started from ttmenu, then
you have to edit ttmenu and replace all occurrences of emacs by the name
of your preferred editor (e.g. nedit or jed).

2) The Turtle input files have to have only a LF (line feed) as line
deliminator. If they are imported from a Windows or VAX environment,
then the line deliminators are CR+LF and Turtle will not work. To
correct this use for example the following procedure via a DOS-formatted
diskette:
		mcopy turtle1.dat a:/
		mcopy -t a:/turtle1.dat

3) If you run Turtle as a non-super user, then you probably will get
"permission denied" errors. To correct this do the following as
super-user in the directory where you have installed turtle:
		chmod 777 *
		chgrp root *
		chown root *
do the same also for .ttmenu.history and the directory of the installation.
continuation in preparation


