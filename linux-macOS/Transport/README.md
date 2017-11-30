# Transport

Version 1.41

```bash
by:    Urs C. Rohrer
       PSI
       Area West
       CH-5232 Villigen-PSI
       Switzerland

email: Urs.Rohrer@psi.ch
phone: +41 56 310 40 35
```

## Transport, a Computer Program for Designing Charged Particle Beam Transport Systems

This version of the computer program Transport is based on a CERN version by
K.L. Brown, D.C. Carey, Ch. Iselin and F. Rothacker (see: CERN 80-04) and is
written in Fortran-77. Only the graphical X-Window interface is written in C.
In order to use the graphical features, you have to start X-windows (startx)
and run all commands from a X-terminal (xterm). Most commands are short C-shell
(csh) scripts. The local printing of the graphical output is done via ghostscript
(gs). Some sample input files are included in this package (.dat). If you choose
one, it has to be copied to FOR001.DAT. The features of the graphical output are
controlled via the file FOR004.DAT. All existing commands are listed when typing
ht:

### Table of different Transport Commands.

|   Comm  Param   |   what it does                                                       |
| --------------- | -------------------------------------------------------------------- |
|   t             |  Transport computation with display of envelope                      |
|  tfo            |  Transport computation with standard output on file                  |
|   s             |  Transport computation without display (only tables)                 |
|   i             |  Transport input file editor for base sequence (mod. FOR001)         |
|  ifq            |  Transport fit sequence editor (modifies FOR001)                     |
|   o             |  Shows output files & editor for display parameters (after T)        |
|   e             |  display beam ellipses (after T)                                     |
| save            |  store envelope for multiple display (after T)                       |
|  dtm   ijk      |  Display of up to 4 Tijk (after T)                                   |
| tijk            |  List of 2nd-order components (after T)                              |
|   si            |  Output of Sigma- & R1-matrices (after T and TFO)                    |
|   el            |  *Elements* - List (after T and TFO)                                 |
|   r             |  New display of envelope (after O etc.)                              |
|  qoi            |  Transfer of Quadrupole fields from output to input (after T)        |
|  loi            |  Transfer of varied element lengths from output to input (after T)   |
|  boi            |  Transfer of initial beam parameters from output to Input (after T)  |
| bero            |  B * Rho scaling of a beamline (modifies FOR001)                     |
| delf            |  Erase all scratch files                                             |
| tpmenu          |  X command shell for transport                                       |
| kdestart        |  Script to start tpmenu from kde desktop (please edit path)          |

## X Command Shell

If you start tpmenu, then a X-window menu containing buttons for almost all
of the above commands is poping up. Tpmenu runs only if the wish-shell
(Tcl/Tk) is present in the system (e.g. in /usr/bin).

## KDE support

In order to be able to start Transport via an icon in the taskbar of
the kde desktop, trans.xpm and kdestart have been included. trans.xpm has
to be copied into the kde icons directory [e.g. /opt/kde/share/icons or
/usr/share/icons] and a kfm link to kdestart has to be established by
dragging and dropping kdestart onto the taskbar. Do not forget to enable
the necessary permissions when you are running as a non-superuser.
kdestart has also to be edited in order to include the directory name of
your installation.

## Hints

1. If you do not like to use emacs as the editor started from tpmenu, then you have to edit tpmenu and replace all occurrences of emacs by the name of your preferred editor (e.g. gedit).
2. The Transport input files have to have only a LF (line feed) as line deliminator. If they are imported from a Windows or VAX environment, then the line delimiters are CR+LF and Transport will not work.
