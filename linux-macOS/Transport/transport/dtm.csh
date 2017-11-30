#!/bin/csh -f

if (! -e FOR000.DAT) then
  echo " Run Transport (2nd order) first !"
  goto exit
endif

./displaw 2ND $argv

exit:
