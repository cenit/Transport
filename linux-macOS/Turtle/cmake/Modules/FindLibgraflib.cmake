
include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CERNLIB_FOUND to TRUE
# if all listed variables are TRUE

find_path(LIBGRAFLIB_INCLUDE_DIR graflib.h hplot.h)
find_library(LIBGRAFLIB_LIBRARY NAMES graflib)
#message("--- Looking for Libgraflib headers into: " "${CERNLIB_INC_SEARCH_PATH}")
#message("--- Looking for Libgraflib library into: " "${CERNLIB_LIB_SEARCH_PATH}")
#message("--- Libgraflib library dir: " "${LIBGRAFLIB_LIBRARY}")
#message("--- Libgraflib header dir: " "${LIBGRAFLIB_INCLUDE_DIR}")
find_package_handle_standard_args(Libgraflib DEFAULT_MSG LIBGRAFLIB_LIBRARY LIBGRAFLIB_INCLUDE_DIR)
mark_as_advanced(LIBGRAFLIB_INCLUDE_DIR LIBGRAFLIB_LIBRARY )

set(LIBGRAFLIB_INCLUDE_DIRS ${LIBGRAFLIB_INCLUDE_DIR} )
set(LIBGRAFLIB_LIBRARIES ${LIBGRAFLIB_LIBRARY} )
