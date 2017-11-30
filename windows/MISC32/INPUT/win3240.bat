@echo off

set CPU=i386

SET PATH=c:\;d:\windows;d:\windows\system32;
set PATH=%path%c:\Visual\VC98\bin\;
set PATH=%path%c:\Visual\DF98\bin\;
set PATH=%path%c:\Visual\Common\MSDev98\bin\;


set LIB=
set LIB=%LIB%c:\Visual\VC98\lib;
set LIB=%LIB%c:\Visual\DF98\lib;
set LIB=%LIB%.\MYWPP\;

set INCLUDE=
set INCLUDE=%INCLUDE%c:\Visual\VC98\INCLUDE;
set INCLUDE=%INCLUDE%c:\Visual\VC98\ATL\INCLUDE;
set INCLUDE=%INCLUDE%c:\Visual\DF98\INCLUDE;
set INCLUDE=%INCLUDE%.\MYWPP\;

