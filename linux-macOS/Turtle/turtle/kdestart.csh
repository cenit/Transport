#!/bin/csh -f

set turtdir = /home/rohrer/turt/
set menu = ./ttmenu

if (-e $turtdir) then
  cd $turtdir
  xterm -sb -sl 500 -j -ls -fn 7x14 -fg black -bg grey -geometry 80x24 -e $menu
else
  echo "turtle directory in kdestart is wrong -- please edit"
endif
