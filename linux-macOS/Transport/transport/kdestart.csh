#!/bin/csh -f

set transdir = /home/rohrer/trans/
set menu = ./tpmenu

if (-e $transdir) then
  cd $transdir
  xterm -sb -sl 500 -j -ls -fn 7x14 -fg black -bg grey -geometry 80x24 -e $menu
else
  echo "transport directory in kdestart is wrong -- please edit"
endif
