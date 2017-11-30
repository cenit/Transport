#!/bin/csh -f

###############################################################################
# vpp is a remote printing program (via tcp/ip) furnished by ethz and widely  #
# used at psi. On other sites it has to be replaced by something else.        #
###############################################################################

# black and white laser copy:

if ($argv[1] == p1) then
#  echo "Printing via VPP"
#  vpp -dev=PS800 $argv[2] >/dev/null
   echo "Printing via CUPS"
   mv $argv[2] temp.ps 
   psresize -W2224 -H3144 -w2224 -h3144 -q temp.ps $argv[2]
   lp -d WLGA_D12_2 $argv[2] >/dev/null
   rm temp.ps
   exit
endif

# colored laser copy:

if ($argv[1] == p2) then
#  echo "Printing via VPP"
#  vpp -dest=PSW01 -dev=PHASER $argv[2] >/dev/null
   echo "Printing via CUPS"
   mv $argv[2] temp.ps 
   psresize -W2224 -H3144 -w2224 -h3144 -q temp.ps $argv[2]
   lp -d WLGA_D12_2 $argv[2] >/dev/null
   rm temp.ps
   exit
endif

# printing with local dev1:

if ($argv[1] == p3) then
  echo "Printing via ghostscript with HP Deskjet 550c."
  gs -q -sDEVICE=cdj550 -sOutputFile=hist.prn $argv[2] <gsprompt >/dev/null
  lpr -l -s -r hist.prn
  echo "Printout queued."
  exit
endif

# printing with local dev2:

if ($argv[1] == p4) then
  echo "Printing via ghostscript with Canon BJ-10e."
  gs -q -sDEVICE=bj10e -sOutputFile=hist.prn $argv[2] <gsprompt >/dev/null
  lpr -l -s -r hist.prn
  echo "Printout queued."
  exit
endif

echo "Unsupported command: printg $argv[1] $argv[2]"
