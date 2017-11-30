#!/bin/csh -f

if ($argv[1] == 2) then
 goto quad
endif

echo " Performing TRANSPORT envelope fit for PIREX."
echo " ============================================"
echo " "

if (-e FOR022.DAT) then
  goto continue
else
  echo " FOR022.DAT not found. Aborting procedure."
  goto exit
endif

continue:

cp envfitpx.dat FOR001.DAT
echo " envfitpx.dat ---> FOR001.DAT"
cp dismodpx.dat FOR004.DAT
echo " dismodpx.dat ---> FOR004.DAT"

./fxydac
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
echo " Performing modification of beam spot size at PIREX location."
echo " ============================================================"
echo " "

cp focuspx.dat FOR001.DAT
echo " focuspx.dat ---> FOR001.DAT"

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
