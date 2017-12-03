#! /bin/bash
#
# Purpose:
#    Fetch and build CERNLIB on macOS
#
# Args:
#    $1  - location of top level directory [default: "/opt/cern/cernlib" ]
#
CERN=$1
if [ -z "$CERN" ] ; then
   CERN=/opt/cern/cernlib
fi
CERN_LEVEL=2005
export CERN CERN_LEVEL

do_lapack=yes
do_compat=yes
do_unpack=yes
do_cernlib=yes
do_nypatchy=yes
clean_download=yes
do_clean=all   # 'none' 'all' 'cernsrc'
stoponerr=yes

rwhtest=nope
if [ "$rwhtest" = "yes" ] ; then
  echo "**************** RWH test config ***************"
  do_lapack=no
  do_compat=no
  do_unpack=yes
  do_cernlib=yes
  do_nypatchy=yes
  stoponerr=yes
  clean_download=no
  do_clean=cernsrc   # 'none' 'all' 'cernsrc'
fi

if [ "$do_lapack"   = "" ] ; then do_lapack=yes ; fi
if [ "$do_compat"   = "" ] ; then do_compat=yes ; fi
if [ "$do_unpack"   = "" ] ; then do_unpack=yes ; fi
if [ "$do_cernlib"  = "" ] ; then do_cernlib=yes ; fi
if [ "$do_nypatchy" = "" ] ; then do_nypatchy=yes ; fi

#
# cernlib build scripts desire some env variables
#
export CERN_ROOT=${CERN}/${CERN_LEVEL}
export CVSCOSRC=${CERN_ROOT}/src
export PATH=${CERN_ROOT}/bin:${PATH}
# make *really* sure that they're exported ... or kablooie
export CERN_ROOT CVSCOSRC PATH CERN CERN_LEVEL

# some build script expect 'gmake'
# alas, this doesn't work at the N-th depth of make itself ...
hasgmake=`which gmake | cut -d' ' -f1`
if [ "$hasgmake" == "no" ] ; then
  function gmake()
  {
     make $*
  }
  # export it to subshells
  export -f gmake
fi
#
# make sure we have somewhere to put it
#
# should already be existing by the README procedure, if not it could fail for permission problems!
if [ ! -d ${CERN} ] ; then mkdir -p ${CERN} ; fi
cd ${CERN}
if [ ! -d ${CERN_LEVEL} ] ; then mkdir ${CERN_LEVEL} ; fi
cd ${CERN_ROOT}

#
# clean out previous tries ...
#
if [ "$clean_downloads" = "yes" ] ; then
   if [ -d downloads ] ; then rm -r downloads ; fi
fi
if [ "$do_clean" = "all" -o "$do_clean" = "cernsrc" ] ; then
    echo "*** clean $do_clean ***"
    do_unpack="yes"
    cd ${CERN_ROOT}
    if [ "$do_clean" = "cernsrc" ] ; then
        dirlist='src'
    else
        dirlist='bin include lib src aux'
    fi
    for subdir in $dirlist ; do
        if [ -d $subdir ] ; then
            chmod -R +w $subdir
            rm -r $subdir
        fi
    done
fi

#
# it is critically important that MAKEFLAGS/MAKEFILES aren't set
# (SRT sets these but they screw up just about all non-SRT builds)
#
if [ "$MAKEFLAGS" != "" ] ; then
  echo "MAKEFLAGS was set; will continue with it unset (be careful about SRT)"
  export SRT_SAVE_MAKEFLAGS=$MAKEFLAGS
  unset MAKEFLAGS
fi
if [ "$MAKEFILES" != "" ] ; then
  echo "MAKEFLAGS was set; will continue with it unset (be careful about SRT)"
  export SRT_SAVE_MAKEFILES=$MAKEFILES
  unset MAKEFILES
fi

#
# create (if necessary) some of the subdirs we'll be using
#
cd ${CERN_ROOT}
for subdir in bin include lib src download aux ; do
  if [ ! -d $subdir ] ; then mkdir $subdir ; fi
done

#
# fetch what we need
#
function fetchit()
{  # usage:  fetchit pathURL
    url=$1
    srcfile=`basename $url`
    srcpath=`dirname $url`
    #echo "fetchit $url -> $srcfile"
    if [ "$srcfile" == "" ] ; then
        echo 'fetchit() missing file name ' ; return ; fi
    if [ "$srcpath" == "" ] ; then
        echo 'fetchit() missing pathURL ' ; return ; fi
    if [ -f $srcfile ] ; then
        echo "Already have \"$srcfile\""
    else
        echo "Fetching $srcfile"
        curl -O $srcpath/$srcfile
        status=$?
        if [ $status != 0 -o ! -f $srcfile ] ; then
            echo "Failed to get $srcfile from $srcpath, status=$status"
            exit
        fi
        echo " "
    fi
}

cd ${CERN_ROOT}/download

fetchit http://www.netlib.org/lapack/lapack-3.1.0.tgz

MOMMSEN="http://mommsen.home.cern.ch/mommsen/osx/cernlib"
fetchit $MOMMSEN/cuserid.c
fetchit $MOMMSEN/cernlib-2002-macosx.patch

fetchit http://home.fnal.gov/~rhatcher/macosx/cernlib-2005-macosx-intel.patch

CERN_SRCTAR="http://cernlib.web.cern.ch/cernlib/download/2005_source/tar"
for pkg in nypatchy_boot gcalor src_cfortran src_cmz src_config \
           src_geant321 src_graflib src_Imakefile src_include \
           src_mathlib src_mclibs src_packlib src_patchy src_pawlib \
           src_phtools src_scripts
do
  fetchit $CERN_SRCTAR/$pkg.tar.gz
done

#
# LAPACK
#
if [ "$do_lapack" != "no" ] ; then
  cd ${CERN_ROOT}/aux
  echo "processing lapack+blas, be patient"
  tar xvzf ${CERN_ROOT}/download/lapack-3.1.0.tgz > /dev/null
  cd lapack-3.1.0
  # customize makefile
  sed -e 's/_LINUX/_DARWIN/' make.inc.example  > make.inc
  echo "" >> make.inc
  echo ".SUFFIXES : .f .o" >> make.inc
  echo "" >> make.inc
  # gfortran deficiency - intrinisic ETIME can't be declared "EXTERNAL"
  cd INSTALL
  if [ ! -f dsecnd.f.save ] ; then mv dsecnd.f dsecnd.f.save ; fi
  if [ ! -f second.f.save ] ; then mv second.f second.f.save ; fi
  sed -e 's/      EXTERNAL/*      EXTERNAL/' dsecnd.f.save > dsecnd.f
  sed -e 's/      EXTERNAL/*      EXTERNAL/' second.f.save > second.f
  cd ..

  # go ahead and build - "all" will perform tests
  make blaslib lapacklib tmglib > make.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make blaslist lapacklib tmglib ***"
      grep -i err make.log
  fi
  # make all

  # copy/rename the libraries
  cp blas_DARWIN.a ${CERN_ROOT}/lib/libblas.a
  cp lapack_DARWIN.a ${CERN_ROOT}/lib/liblapack3.a

fi

#
# compat library
#
if [ "$do_compat" != "no" ] ; then
  cd ${CERN_ROOT}/aux
  if [ ! -d compat ] ; then mkdir compat ; fi
  cd compat
  echo "generate compat library"
  cp ${CERN_ROOT}/download/cuserid.c .
  cc -O -fPIC -c cuserid.c
  cat > getarg.c <<EOF
#include <stdio.h>
#include <string.h>
/*
 * gfortran transforms the INSTRINIC GETARG into __gfortran_getarg_i4
 * so we'll supply our own
 */
void getarg_(int* pos, char* value, size_t len)
{
  getarg_stub_(pos,value,len);
}
int  iargc_()
{
  return iargc_stub_();
}
EOF
  cc -O -fPIC -c getarg.c
  cat > getarg_stub.f <<EOF
      subroutine getarg_stub( i, s )
      implicit none
      integer i
      character(*) s
c gfortran transforms this INTRINSIC into something different
c so one can't directly call it from C
      intrinsic GETARG
      call GETARG(i,s)
      return
      end
      integer function iargc_stub()
      implicit none
      intrinsic IARGC
      iargc_stub = IARGC()
      return
      end
EOF
  gfortran   -fPIC -c getarg_stub.f
  ar cr  ${CERN_ROOT}/lib/libcompat.a cuserid.o getarg.o getarg_stub.o
  ranlib ${CERN_ROOT}/lib/libcompat.a
  rm cuserid.o getarg.o getarg_stub.o
fi

#
# unpack the CERNLIB source
# tar files expect to be at the "cernlib" directory level,
#   i.e. they start with "2005" as the first subdir
# then apply the patches
#
if [ "$do_unpack" != "no" ] ; then
  echo "unpack source .tar.gz"
  # if unpacking, just clean out the old source first
#  if [ -d ${CERN_LEVEL}/src ] ; then
#    cd ${CERN_LEVEL}
#    chmod -R +w src
#    rm -r src
#    cd ..
#  fi
  cd ${CERN}
  ls -1 ${CERN_ROOT}/download/src_*.tar.gz | xargs -n1 tar xzf
  echo "untar done"

  cd ${CERN_ROOT}/src
  echo "apply patches"
  patch -p0 < ${CERN_ROOT}/download/cernlib-2005-macosx-intel.patch
  echo "done patching"
fi

#
# build CERNLIB
#
if [ "$do_cernlib" != "no" ] ; then
  echo "start to build cernlib proper"
  cd ${CERN_ROOT}/src

  # eliminate references to 'gmake'
  offender=config/site.def
  echo "modify $offender 'gmake' -> 'make'"
  mv $offender ${offender}.with_gmake
  sed -e 's/gmake/make/g' ${offender}.with_gmake > ${offender}

  echo "imake_boot"
  $CVSCOSRC/config/imake_boot

  # (for the curious) list of predefined cpp macros
  ## cpp -dM < /dev/null

if [ 0 > 1 ] ; then
  echo "about to cleanup references to 'gmake'"
cat > sedgmake <<EOF
#! /bin/bash
# substitute 'make' for 'gmake'
  fname=\$1
  ngmake=\`grep -c gmake \$fname\`
#  echo "sedgmake \$fname \$ngmake"
  if [ "\$ngmake" != "0" ] ; then
       echo "sedgmake: modify \$fname"
       mv \$fname \${fname}.with_gmake
       sed -e 's/gmake/make/g' \${fname}.with_gmake > \$fname
   fi
EOF
chmod +x ./sedgmake
#cat sedgmake
echo "eliminate 'gmake' references in makefiles"
##  find . -name '*ake*' -exec ./sedgmake {} \;
fi

  # despite the mommsen instructions having it reversed we build kuipc first
  echo "begin make bin/kuipc"
  make bin/kuipc > make.kuipc.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make bin/kuipc ***"
      grep -i err make.kuipc.log
      if [ "$stoponerr" == "yes" ] ; then exit ; fi
  fi
  echo "begin general make (this one takes a long time)"
  make > make.pass1.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make (basic 'pass1' ) ***"
      grep -i err make.pass1.log
      if [ "$stoponerr" == "yes" ] ; then exit ; fi
  fi


  echo "done general make, build scripts"
  make scripts/Makefile
  cd scripts
  make install.bin > ../make.scripts.bin.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make scripts/install.bin ***"
      grep -i err ../make.script.bin.log
      if [ "$stoponerr" == "yes" ] ; then exit ; fi
  fi

  echo "begin packlib install.bin"
  cd ../packlib
  make install.bin > ../make.packlib.bin.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make packlib/install.bin ***"
      grep -i err ../make.packlib.bin.log
      if [ "$stoponerr" == "yes" ] ; then exit ; fi
  fi

  echo "begin graflib install.bin"
  cd ../graflib
  make install.bin > ../make.graflib.bin.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make graflib/install.bin ***"
      grep -i err ../make.graflib.bin.log
      if [ "$stoponerr" == "yes" ] ; then exit ; fi
  fi

  echo "begin pawlib install.bin"
  cd ../pawlib
  make install.bin > ../make.pawlib.bin.log 2>&1
  if [ $? != 0 ] ; then
      echo "*** Error in make pawlib/install.bin ***"
      grep -i err ../make.pawlib.bin.log
  fi

  #nope# cd ..
  #nope# make install.include

fi


#
# build a bootstrap "ypatchy" (don't bother if we have a nypatchy )
#
if [ "$do_nypatchy" != "no" ] ; then
  if [ ! -f ${CERN_ROOT}/bin/nypatchy ] ; then
    cd ${CERN_ROOT}/aux
    echo "build the bootstrap ypatchy"
    tar xvzf ${CERN_ROOT}/download/nypatchy_boot.tar.gz > /dev/null
    cd p5boot
    cd p5lib
    gfortran -c *.f > gfortran.log 2>&1
    ar cr ../libp5.a *.o
    cd ..
    ranlib libp5.a
    gfortran -o nypatchy nypatchy.f libp5.a -L$CERN_ROOT/lib/ -lkernlib
    cp nypatchy $CERN_ROOT/bin
    cd ${CERN_ROOT}/bin
    ln -s nypatchy ypatchy
  fi
fi

#
# build the real ypatchy executables (must have a copy of nypatchy)
#
if [ "$do_nypatchy" != "no" ] ; then
  if [ -f ${CERN_ROOT}/bin/nypatchy ] ; then
    cd ${CERN_ROOT}/src/patchy
    echo "build the real patchy (and friends)"
    # missing nytobin.cra ?   apparently now missing from the source as well
#    if [ ! -f nytobin.cra ] ; then
#       sed -e 's/YTIDY,  D=NTIDY/YTOBIN,  D=NYTOBIN/' nytidy.cra > nytobin.cra
#    fi
    imake -DUseInstalled -I$CVSCOSRC/config \
        -DTOPDIR=`/bin/pwd`/.. -DCURDIR=/patchy -fImakefile
    make > make.log 2>&1
    if [ $? != 0 ] ; then
        echo "*** Error in make ***"
        grep -i err make.log
    fi
    make install.bin > make.install.log 2>&1
    if [ $? != 0 ] ; then
        echo "*** Error in make install.bin ***"
        grep -i err make.install.log
    fi
  fi
fi

cd ${CERN_ROOT}

echo "==============================================="
echo "Some necessary env variables:"
echo "==============================================="
echo "export CERN=${CERN}"
echo "export CERN_LEVEL=${CERN_LEVEL}"
echo "export CERN_ROOT=\${CERN}/${CERN_LEVEL}"
echo "export CERN_DIR=\${CERN_ROOT}"
echo "export CVSCOSRC=\${CERN_ROOT}/src"
echo "export CERNLIBS=\${CERN_DIR}/lib"
echo "export CERNLIB=\${CERN_DIR}/lib"
echo "export CRNLIB=\${CERN_DIR}/lib"
echo "export GEANT_DIR=\${CERN_DIR}"
echo "export PATH=\${CERN_DIR}/bin:\${PATH}"
echo "==============================================="
#
# End-of-Script
