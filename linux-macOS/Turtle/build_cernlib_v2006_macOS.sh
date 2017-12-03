#! /bin/bash
#
# Purpose:
#    Fetch and build CERNLIB on macOS
#
# Args:
#    $1  - location of top level directory [default: "/opt/cern/cernlib" ]
#
export CC=/usr/local/bin/gcc-7
export CXX=/usr/local/bin/g++-7
export FC=/usr/local/bin/gfortran-7

CERN=$1
if [ -z "$CERN" ] ; then
   CERN=/opt/cern/cernlib
fi
CERN_LEVEL=2006
export CERN CERN_LEVEL

#
# cernlib build scripts desire some env variables
#
export CERN_ROOT=${CERN}/${CERN_LEVEL}
export CVSCOSRC=${CERN_ROOT}/src
export PATH=${CERN_ROOT}/bin:${PATH}
# make *really* sure that they're exported ... or kablooie
export CERN_ROOT CVSCOSRC PATH CERN CERN_LEVEL

if [ ! -d ${CERN} ] ; then echo "Missing required folder: ${CERN}" ; exit ; fi
if [ ! -d ${CERN_ROOT} ] ; then mkdir -p ${CERN_ROOT} ; fi
cd ${CERN_ROOT} || exit

#
# clean out previous tries ...
#
if [ -d downloads ] ; then rm -r downloads ; fi

cd ${CERN_ROOT} || exit
dirlist='bin include lib src aux'
for subdir in $dirlist ; do
    if [ -d "$subdir" ] ; then
        chmod -R +w "$subdir"
        rm -r "$subdir"
    fi
done

#
# create (if necessary) some of the subdirs we'll be using
#
cd ${CERN_ROOT} || exit
for subdir in bin include lib src download aux ; do
  if [ ! -d $subdir ] ; then mkdir -p $subdir ; fi
done

#
# fetch what we need
#
function fetchit()
{  # usage:  fetchit pathURL
    url=$1
    srcfile=$(basename "$url")
    srcpath=$(dirname "$url")
    #echo "fetchit $url -> $srcfile"
    if [ "$srcfile" == "" ] ; then
        echo 'fetchit() missing file name ' ; return ; fi
    if [ "$srcpath" == "" ] ; then
        echo 'fetchit() missing pathURL ' ; return ; fi
    if [ -f "$srcfile" ] ; then
        echo "Already have \"$srcfile\""
    else
        echo "Fetching $srcfile"
        curl -O "$srcpath/$srcfile"
        status=$?
        if [ $status != 0 ] || [ ! -f "$srcfile" ] ; then
            echo "Failed to get $srcfile from $srcpath, status=$status"
            exit
        fi
        echo " "
    fi
}

cd ${CERN_ROOT}/download || exit

CERN_SRCTAR="http://cernlib.web.cern.ch/cernlib/download/2006_source/tar"
for pkg in 2006_src include mathlib32_src minuit32_src gcalor nypatchy_boot
do
  fetchit $CERN_SRCTAR/$pkg.tar.gz
done

#fetchit http://www-jlc.kek.jp/~fujiik/macosx/10.12.X/HEPonX/SOURCES/cernlib/cernlib-2006-macosx10.9.patch
fetchit http://www-jlc.kek.jp/~fujiik/macosx/10.12.X/HEPonX/SOURCES/cernlib/cernlib-2006-macosx10.11.patch
fetchit http://www-jlc.kek.jp/~fujiik/macosx/10.12.X/HEPonX/SOURCES/cernlib/cernlib2006-2006b-14.patch

#
# unpack the CERNLIB sources
#
echo "unpack source .tar.gz"
cd ${CERN} || exit
for pkg in 2006_src include mathlib32_src minuit32_src
do
  tar zxf ${CERN_ROOT}/download/$pkg.tar.gz
done
echo "untar done"



ln -s ${CERN_LEVEL} new
ln -s ${CERN_LEVEL} pro
mkdir -p ${CERN_LEVEL}/work
cd ${CERN_LEVEL}/src || exit
patch -p1 -s < ${CERN_ROOT}/download/cernlib2006-2006b-14.patch
#patch -p1 -s < ${CERN_ROOT}/download/cernlib-2006-macosx10.9.patch
patch -p1 -s < ${CERN_ROOT}/download/cernlib-2006-macosx10.11.patch
cd ${CERN_ROOT} || exit
mkdir -p lib
ln -s /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libLAPACK.dylib lib/liblapack3.a
ln -s /System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/libBLAS.dylib lib/libblas.a
cd work || exit
export CVSCOSRC=${CERN_ROOT}/src
mkdir -p ../bin
cp $CVSCOSRC/scripts/cernlib ../bin/
$CVSCOSRC/config/imake_boot
export PATH=$PATH:${CERN_ROOT}/bin
mkdir -p ../logs
make tree HAVE_MOTIF='YES' >& ../logs/tree.log &
pushd ../src/packlib/kuip/programs/kuipc || exit
make
make install.bin
popd || exit
make HAVE_MOTIF='YES' >& ../logs/make.log &
cd ${CERN_ROOT}/work/packlib || exit
make install.bin HAVE_MOTIF=YES PACKAGE_LIB='/cern/pro/lib/libpacklib.a' EXTRA_LOAD_FLAGS='-L/opt/osxws/lib -lgfortran' >& ../../logs/packlib.bin.log &
cd ../pawlib || exit
make install.bin HAVE_MOTIF=YES PACKAGE_LIB='/cern/pro/lib/libpawlib.a' EXTRA_INCLUDES='-I/cern/2006/src/pawlib/comis' >& ../../logs/pawlib.bin.log &
cd ../graflib || exit
make install.bin HAVE_MOTIF=YES PACKAGE_LIB='/cern/pro/lib/libgraflib.a' >& ../../logs/graflib.bin.log &
cd ../scripts || exit
make install.bin HAVE_MOTIF='YES' >& ../../logs/scripts.bin.log &
cd .. && make install.include CERN_INCLUDEDIR=/cern/new/include >& ../logs/install.include.log &

## TESTS
# cd packlib
# make test PACKAGE_LIB='/cern/pro/lib/libpacklib.a' >& ../../logs/packlib.test.log &
# cd ../mathlib
# make test PACKAGE_LIB='/cern/pro/lib/libmathlib.a' >& ../../logs/mathlib.test.log &
# cd ../graflib/higz/examples
# make higzex PACKAGE_LIB='/cern/pro/lib/libgraflib.a'
# ./higzex
# cd ../../../../src/pawlib/paw/demo
# paw all.kumac
# cd ../../../../work/mclibs
# FC='gfortran -O0' make test >& ../../logs/mclibs.test.log &
# tail -f ../../logs/mclibs.test.log
# cd ../phtools
# make test >& ../../logs/phtools.test.log &
# tail -f ../../logs/phtools.test.log
# cd ../geant321
# make test EXTRA_LOAD_FLAGS='-undefined dynamic_lookup' >& ../../logs/geant321.test.log &
# tail -f ../../logs/geant321.test.log
