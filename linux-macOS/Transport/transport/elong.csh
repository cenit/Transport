#!/bin/csh -f

if ( ! -e FOR002.DAT ) then
 echo " run Transport first !"
 goto exit
endif

./displaw L

exit:




