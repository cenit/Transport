cmake_minimum_required (VERSION 2.8.7)
project(Turtle)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(APPLE)
    set (CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "/opt/cern/cernlib")
endif

enable_language(Fortran)
enable_language(C)

set (CMAKE_Fortran_FORMAT FIXED)

find_package(Curses REQUIRED)
find_package(Libgraflib REQUIRED)
find_package(LibgrafX11 REQUIRED)
find_package(Libkernlib REQUIRED)
find_package(Libmathlib REQUIRED)
find_package(Libpacklib REQUIRED)
find_package(Libphtools REQUIRED)
find_package(X11 REQUIRED)

include_directories(${CURSES_INCLUDE_DIR} ${X11_INCLUDE_DIR} ${LIBGRAFLIB_INCLUDE_DIR} ${LIBGRAFX11_INCLUDE_DIR} ${LIBKERNLIB_INCLUDE_DIR} ${LIBMATHLIB_INCLUDE_DIR} ${LIBPACKLIB_INCLUDE_DIR} )

# make sure that the default is a RELEASE
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

add_executable (turtle
    sources/turtlin.f
    sources/system.c
)

add_executable (muscat
    sources/muscat.f
    sources/system.c
)

add_executable(openURL  sources/openURL.c)

add_executable(gratur
    gratur/gratur.f
    gratur/errno.c
    gratur/ctype.c
)

target_link_libraries(turtle ${CURSES_LIBRARY})
target_link_libraries(muscat ${CURSES_LIBRARY})
target_link_libraries(gratur ${LIBGRAFLIB_LIBRARIES} ${LIBGRAFX11_LIBRARIES} ${LIBKERNLIB_LIBRARIES} ${LIBMATHLIB_LIBRARIES} ${LIBPACKLIB_LIBRARIES} ${LIBPHTOOLS_LIBRARIES} ${X11_LIBRARIES})

install (TARGETS
    turtle
    muscat
    gratur
    openURL
    DESTINATION ${CMAKE_SOURCE_DIR}/bin/
)
