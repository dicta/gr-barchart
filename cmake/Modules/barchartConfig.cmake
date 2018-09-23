INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_BARCHART gnuradio-barchart)

FIND_PATH(
    BARCHART_INCLUDE_DIRS
    NAMES barchart/api.h
    HINTS $ENV{BARCHART_DIR}/include
        ${PC_BARCHART_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    BARCHART_LIBRARIES
    NAMES gnuradio-barchart
    HINTS $ENV{BARCHART_DIR}/lib
        ${PC_BARCHART_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BARCHART DEFAULT_MSG BARCHART_LIBRARIES BARCHART_INCLUDE_DIRS)
MARK_AS_ADVANCED(BARCHART_LIBRARIES BARCHART_INCLUDE_DIRS)
