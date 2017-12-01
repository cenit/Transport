
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE


find_path(LIBGRAFX11_INCLUDE_DIR higz.h )
find_library(LIBGRAFX11_LIBRARY NAMES grafX11)
find_package_handle_standard_args(LibgrafX11 DEFAULT_MSG LIBGRAFX11_LIBRARY LIBGRAFX11_INCLUDE_DIR)
mark_as_advanced(LIBGRAFX11_INCLUDE_DIR LIBGRAFX11_LIBRARY )

set(LIBGRAFX11_INCLUDE_DIRS ${LIBGRAFX11_INCLUDE_DIR} )
set(LIBGRAFX11_LIBRARIES ${LIBGRAFX11_LIBRARY} )
