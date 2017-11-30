
Instructions for building Transport & Turtle for Windows.
=========================================================

On this CD all sources for building
"Transport for Windows"
and
"Turtle for Windows"
are included.

You need to have 
"Digital Visual C++ Version 6" (on a separate CD)
and
"Digital Visual Fortran Version 6.0A" (on a separate CD)
installed on your Windows Computer.

Also included are 2 directories ("Trans" & "Turtle") with a minimal user distribution
for running both packages. (For startup use the Shortcuts 'Transport' or 'Turtle')

First copy the Folder Trans-Turtle on this CD to c:\

In order to compile and link the diverse *.exe and *.dll files you have to
install C++ and Fortran from 2 different CDs. In order to use the make- and bat-
files without editing, you have to use the following directories during the
installation of the compilers:

Enter as path C:\Visual. This produces the following directory tree on yout PC:

C:\Visual\Common   (for C++ and Fortran)
C:\Visual\DF98     (for Fortran)
C:\Visual\VC98     (for C++)

Now you are ready to run Makeall.bat from the command Window (cmd). After about
a minute all modules are built and the corresponding files in Trans and Turtle
are updated.
