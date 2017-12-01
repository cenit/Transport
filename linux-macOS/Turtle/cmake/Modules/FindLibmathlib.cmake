
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE


find_path(LIBMATHLIB_INCLUDE_DIR gen.h )
find_library(LIBMATHLIB_LIBRARY NAMES mathlib PATH_SUFFIXES lib64 libx32)
find_package_handle_standard_args(Libmathlib DEFAULT_MSG LIBMATHLIB_LIBRARY LIBMATHLIB_INCLUDE_DIR)
mark_as_advanced(LIBMATHLIB_INCLUDE_DIR LIBMATHLIB_LIBRARY )

set(LIBMATHLIB_INCLUDE_DIRS ${LIBMATHLIB_INCLUDE_DIR} )
set(LIBMATHLIB_LIBRARIES ${LIBMATHLIB_LIBRARY} )
