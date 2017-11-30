#!/bin/csh -f
mv FOR001.DAT for001.dat
mv FOR004.DAT for004.dat
if (-e FOR022.DAT) then
  mv FOR022.DAT for022.dat
endif
rm -f *.DAT*
mv for001.dat FOR001.DAT
mv for004.dat FOR004.DAT
if (-e for022.dat) then
  mv for022.dat FOR022.DAT
endif
if (-e ERROR.TRA) then
  rm -f ERROR.TRA
endif
if (-e TRANS.PS) then
  rm -f TRANS.PS
endif




