#!/bin/csh -f

if ( ! -e FOR002.DAT ) then
 echo " run Transport first !"
 goto exit
endif

./plota

if ($status != 0) then
 goto exit
endif

./displaw

exit:




