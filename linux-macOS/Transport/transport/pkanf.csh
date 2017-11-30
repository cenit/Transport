#!/bin/csh -f

if ($argv[1] == 2) then
 goto quad
endif

echo " Performing TRANSPORT envelope fit Pkanal (EHT)."
echo " ==============================================="
echo " "

if (-e FOR022.DAT) then
  goto continue
else
  echo " FOR022.DAT not found. Aborting procedure."
  goto exit
endif

continue:

cp envfitpk.dat FOR001.DAT
echo " envfitpk.dat ---> FOR001.DAT"
cp dismodpk.dat FOR004.DAT
echo " dismodpk.dat ---> FOR004.DAT"

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
echo " Performing modification of beam size at EHT location."
echo " ====================================================="
echo " "

cp breiter.dat FOR001.DAT
echo " breiter.dat ---> FOR001.DAT"

./qoi
if ($status != 0) then
 goto exit
endif

./boi
if ($status != 0) then
 goto exit
endif

./io PIREX
if ($status != 0) then
 goto exit
endif

./t

./o

exit:
