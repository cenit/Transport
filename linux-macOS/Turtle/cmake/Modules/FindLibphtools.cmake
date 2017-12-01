
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE


find_library(LIBPHTOOLS_LIBRARY NAMES phtools PATH_SUFFIXES lib64 libx32 /usr/lib/x86_64-linux-gnu)
find_package_handle_standard_args(Libphtools DEFAULT_MSG LIBPHTOOLS_LIBRARY)
mark_as_advanced(LIBPHTOOLS_LIBRARY )

set(LIBPHTOOLS_LIBRARIES ${LIBPHTOOLS_LIBRARY} )
