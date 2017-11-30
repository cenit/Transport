#!/bin/csh -f

./trans 0

if ($status != 0) then
 goto exit
endif

./plotf

exit:




