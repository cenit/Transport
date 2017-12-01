
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE


find_path(LIBKERNLIB_INCLUDE_DIR kernlib.h )
find_library(LIBKERNLIB_LIBRARY NAMES kernlib PATH_SUFFIXES lib64 libx32)
find_package_handle_standard_args(Libkernlib DEFAULT_MSG LIBKERNLIB_LIBRARY LIBKERNLIB_INCLUDE_DIR)
mark_as_advanced(LIBKERNLIB_INCLUDE_DIR LIBKERNLIB_LIBRARY )

set(LIBKERNLIB_INCLUDE_DIRS ${LIBKERNLIB_INCLUDE_DIR} )
set(LIBKERNLIB_LIBRARIES ${LIBKERNLIB_LIBRARY} )
