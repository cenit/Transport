#!/bin/csh -f

###############################################################################
# vpp is a remote printing program (via tcp/ip) furnished by ethz and widely  #
# used at psi. On other sites it has to be replaced by something else.        #
###############################################################################

# black and white laser copy (L):

if ($argv[1] == bw) then
#  echo "Printing via VPP"
#  vpp -dev=PS800 $argv[2] >/dev/null
   echo "Printing via CUPS"
   lp -d WLGA_D12_2 $argv[2]
endif

# black and white laser copy with initial beam and QDACs (PX):

if ($argv[1] == px) then
   echo "Printing via CUPS"
   lp -d WLGA_D12_2 $argv[2]
endif

# colored laser copy (C):

if ($argv[1] == c) then
  echo "Printing via VPP"
  vpp -dest=PSW01 -dev=PHASER $argv[2] >/dev/null
endif

# printing with local dev1 (P1):

if ($argv[1] == dev1) then
  echo " Printing via ghostscript with HP Deskjet 550c."
  gs -q -sDEVICE=cdj550 -sOutputFile=f.p $argv[2] <gsprompt >/dev/null
  lpr -l -s -r f.p
  echo " Printout queued."
endif

# printing with local dev2 (P2):

if ($argv[1] == dev2) then
  echo " Printing via ghostscript with Canon BJ-10e."
  gs -q -sDEVICE=bj10e -sOutputFile=f.p $argv[2] <gsprompt >/dev/null
  lpr -l -s -r f.p
  echo " Printout queued."
endif

# printing from commands o and si:


if ($argv[1] == text) then
  if ( -e tpmenu.is.running ) then
    ./dialogs selprint
    if ($status == 0) then
      lpr $argv[2]
    else
      lp -d WLGA_D12_2 $argv[2]
    endif
  else
    echo "r for remote or l for local printer (default)."
    set answer = ($<)
    switch ($answer)
         case r:
           lp -d WLGA_D12_2 $argv[2]
           breaksw
         case l:
         default:
           lpr $argv[2]
           breaksw
    endsw
  endif
endif

# converting to gif:

if ($argv[1] == gif) then
  echo " Converting $argv[2] to gif file"
  gs -q -sDEVICE=gif8 -sOutputFile=trans.gif $argv[2] <gsprompt >/dev/null
endif
