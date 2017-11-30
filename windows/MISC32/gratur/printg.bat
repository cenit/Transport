@echo off
rem ############################################################################
rem # vpp is a remote printing program (via tcp/ip) furnished by ethz and      #
rem # widely used at psi. On other sites it has to be replaced by something    #
rem # else.        							       #
rem # lpr is a service, which has to be installed on Windows NT systems. See   #
rem # help from the explorer and under index type lpr.			       #
rem ############################################################################

if %1. == p1. goto vpp1
if %1. == p2. goto vpp2
if %1. == p3. goto dev1
if %1. == p4. goto dev2
goto end

rem monochrom PS laser copy:

:vpp1
echo "Printing via VPP."
vpp -dev=PS800 %2
echo "Printout queued."
goto end

rem colored PS laser copy:

:vpp2
echo "Printing via VPP."
vpp -dest=PSW01 -dev=PHASER %2
echo "Printout queued."
goto end

rem printing with local dev1 on pc310:
rem works only under Windows NT (lpr) and needs Ghostscript (GS)

:dev1
echo "Printing via ghostscript with HP Deskjet 550c."
echo "Enter quit for terminating GS."
c:\gsnt\gsnt -q -Ic:\gsnt -sDEVICE=cdj550 -sOutputFile=cdj550.prn %2
lpr -S pc310 -P cdj550 -o l cdj550.prn
echo "Printout queued."
goto end

rem printing with local dev2 on pc310:
rem works only under Windows NT (lpr) and needs Ghostscript (GS)

:dev2
echo "Printing via ghostscript with Canon BJ-10e."
echo "Enter quit for terminating GS."
c:\gsnt\gsnt -q -Ic:\gsnt -sDEVICE=bj10e -sOutputFile=bj10e.prn %2
lpr -S pc310 -P bj10e -o l bj10e.prn
echo "Printout queued."
goto end

:end

