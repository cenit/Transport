cd transnt
call transnt.bat
cd ..

cd turtlent
call turtlent.bat
cd ..

cd trans32
nmake
cd ..

cd turtle32
nmake
cd ..

cd Misc32

cd bli
call mbli.bat
cd ..

cd gratur
call mgratur.bat
cd ..

cd IFQ
call mifq.bat
cd ..

cd INPUT
call minput.bat
cd ..

cd muscat
call mmuscat.bat
cd ..

cd ..
cd hilib

cd anamtr
nmake
cd ..

cd Camac
nmake
cd ..

cd COLORBUT
nmake
cd ..

cd editor32
nmake
cd ..

cd LISTHSCR
nmake
cd ..

cd MSGDISPI
nmake
cd ..

cd MYWPP
nmake
cd ..

cd ROTKNOB
nmake
cd ..

cd tgie
nmake
cd ..

cd trans
nmake
cd ..

cd TURTLE
nmake
cd ..

cd userbtn
nmake
cd ..

cd ..

copy trans32.dll .\Trans
copy transnt.exe .\Trans
copy .\Hilib\editor32.exe .\Trans
copy .\Hilib\tpmenu32.exe .\Trans
copy .\Hilib\tgie32.exe .\Trans
copy .\Hilib\camac32.dll .\Trans
copy .\Hilib\colrbt32.dll .\Trans
copy .\Hilib\lsthsc32.dll .\Trans
copy .\Hilib\usrbtn32.dll .\Trans
copy .\Misc32\bli32.exe .\Trans
copy .\Misc32\ifq32.exe .\Trans
copy .\Misc32\input32.exe .\Trans



copy turtle32.dll .\Turtle
copy turtlent.exe .\Turtle
copy .\Hilib\editor32.exe .\Turtle
copy .\Hilib\ttmenu32.exe .\Turtle
copy .\Hilib\tgie32.exe .\Turtle
copy .\Hilib\anamtr32.dll .\Turtle
copy .\Hilib\colrbt32.dll .\Turtle
copy .\Hilib\lsthsc32.dll .\Turtle
copy .\Hilib\rotknb32.dll .\Turtle
copy .\Hilib\usrbtn32.dll .\Turtle
copy .\Misc32\gratur32.exe .\Turtle
copy .\Misc32\muscat32.exe .\Turtle



