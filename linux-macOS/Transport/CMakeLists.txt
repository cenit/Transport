cmake_minimum_required (VERSION 2.8.7)
project(Transport)
enable_language(Fortran)
enable_language(C)

set (CMAKE_Fortran_FORMAT FIXED)

find_package(X11 REQUIRED)

include_directories(${X11_INCLUDE_DIR})

# make sure that the default is a RELEASE
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

add_executable (trans 
    sources/translin.f
    sources/system.c
)

add_executable(io
    sources/io.f
    sources/bero.f
    sources/qoi.f
    sources/boi.f
    sources/loi.f
    sources/qoi2.f
    sources/fxydac.f
    sources/fxygauss.f
    sources/pirexf.f
    sources/stat.c
)

add_executable(plotf
    sources/plotf.f
    sources/plot1f.f
    sources/splint.f
    sources/time.c
)

add_executable(plota
    sources/plota.f
    sources/plot1f.f
    sources/splint.f
    sources/time.c
)

add_executable(dispo    sources/dispo.f)
add_executable(disponew sources/disponew.f)
add_executable(element  sources/element.f)
add_executable(el       sources/el.f)
add_executable(emu      sources/emu.f)
add_executable(ifq      sources/ifq.f)
add_executable(si       sources/si.f)
add_executable(siun     sources/siun.f)
add_executable(sigma    sources/sigma.f)
add_executable(tijk     sources/tijk.f)
add_executable(bli      sources/bli.f)
add_executable(openURL  sources/openURL.c)

add_executable(displaw
    display/displaw.f
    display/displaps.f
    display/eliw.f
    display/elips.f
    display/eliw2.f
    display/elips2.f
    display/eliw3.f
    display/elips3.f
    display/dtmw.f
    display/dtmps.f
    display/pssubs.f
    display/bedacps.f
    display/begaussps.f
    display/inips.c
    display/xwi.c
    display/mysleep.c
)

target_link_libraries(displaw ${X11_LIBRARIES})

install (TARGETS 
    trans
    io
    plotf
    plota
    dispo
    disponew
    element
    el
    emu
    ifq
    si
    siun
    sigma
    tijk
    bli
    openURL
    displaw 
    DESTINATION ${CMAKE_SOURCE_DIR}/bin/
)
