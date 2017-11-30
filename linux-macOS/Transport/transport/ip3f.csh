#!/bin/csh -f

if ($argv[1] == 2) then
 goto quad
endif

echo " Performing TRANSPORT envelope fit for IP2 (40 MeV)."
echo " ==================================================="
echo " "

if (-e FOR022.DAT) then
  goto continue
else
  echo " FOR022.DAT not found. Aborting procedure."
  goto exit
endif

continue:

cp envfitip3.dat FOR001.DAT
echo " envfitip3.dat ---> FOR001.DAT"
cp dismodip3.dat FOR004.DAT
echo " dismodip3.dat ---> FOR004.DAT"

./fxygauss
if ($status != 0) then
 goto exit
endif

./t

if ($argv[1] == 1) then
 ./e
 goto exit
endif


quad:

echo " "
echo " Performing modification of beam spot size at IP2-Target location."
echo " ================================================================="
echo " "

cp focusip3.dat FOR001.DAT
echo " focusip3.dat ---> FOR001.DAT"

./qoi
if ($status != 0) then
 goto exit
endif

./boi
if ($status != 0) then
 goto exit
endif

./t

./o

exit:
