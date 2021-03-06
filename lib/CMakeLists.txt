# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file was generated by gr_modtool, a tool from the GNU Radio framework
# This file is a part of gr-barchart
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(
    ${Boost_INCLUDE_DIR}
    ${QWT_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${QWT_LIBRARY_DIRS}
    ${QT_LIBRARY_DIRS}
)

list(APPEND barchart_libs
    ${GNURADIO_ALL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${QWT_LIBRARIES}
    ${QT_LIBRARIES}
)

# Set up Qt4 stuff for MOC/UIC code generation
set(barchart_mod_includedir ${CMAKE_CURRENT_SOURCE_DIR}/../include/barchart)
set(barchart_moc_hdrs
    ${barchart_mod_includedir}/displayform.h
    ${barchart_mod_includedir}/vectordisplayform.h
    ${barchart_mod_includedir}/VectorDisplayPlot.h
    qcustomplot/qcustomplot.h
)
include_directories(${barchart_mod_includedir})
QT4_WRAP_CPP(barchart_moc_sources ${barchart_moc_hdrs})
# Uncomment if we use any UI layouts defined by Qt Creator
# QT4_WRAP_UI(barchart_ui_hdrs)

# Now actually define the source files to build
list(APPEND barchart_sources
    ${barchart_moc_sources}
    ${barchart_ui_hdrs}
    qcustomplot/qcustomplot.cpp
    displayform.cc
    VectorDisplayPlot.cc
    vectordisplayform.cc
    vector_sink_f_impl.cc
)

set(barchart_sources "${barchart_sources}" PARENT_SCOPE)
if(NOT barchart_sources)
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return()
endif(NOT barchart_sources)

add_definitions(-DQWT_DLL)
add_library(gnuradio-barchart SHARED ${barchart_sources})
target_link_libraries(gnuradio-barchart ${barchart_libs})
set_target_properties(gnuradio-barchart PROPERTIES DEFINE_SYMBOL "gnuradio_barchart_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-barchart PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-barchart RUNTIME_COMPONENT "barchart_runtime" DEVEL_COMPONENT "barchart_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_barchart_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_barchart.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_barchart.cc
)

add_executable(test-barchart ${test_barchart_sources})

target_link_libraries(
    test-barchart
    ${GNURADIO_RUNTIME_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CPPUNIT_LIBRARIES}
    gnuradio-barchart
)

GR_ADD_TEST(test_barchart test-barchart)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")
