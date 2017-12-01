
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE


find_path(LIBPACKLIB_INCLUDE_DIR cspack.h )
find_library(LIBPACKLIB_LIBRARY NAMES packlib PATH_SUFFIXES lib64 libx32)
find_package_handle_standard_args(Libpacklib DEFAULT_MSG LIBPACKLIB_LIBRARY LIBPACKLIB_INCLUDE_DIR)
mark_as_advanced(LIBPACKLIB_INCLUDE_DIR LIBPACKLIB_LIBRARY )

set(LIBPACKLIB_INCLUDE_DIRS ${LIBPACKLIB_INCLUDE_DIR} )
set(LIBPACKLIB_LIBRARIES ${LIBPACKLIB_LIBRARY} )
