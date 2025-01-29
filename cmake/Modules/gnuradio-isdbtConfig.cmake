find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_ISDBT gnuradio-isdbt)

FIND_PATH(
    GR_ISDBT_INCLUDE_DIRS
    NAMES gnuradio/isdbt/api.h
    HINTS $ENV{ISDBT_DIR}/include
        ${PC_ISDBT_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_ISDBT_LIBRARIES
    NAMES gnuradio-isdbt
    HINTS $ENV{ISDBT_DIR}/lib
        ${PC_ISDBT_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-isdbtTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_ISDBT DEFAULT_MSG GR_ISDBT_LIBRARIES GR_ISDBT_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_ISDBT_LIBRARIES GR_ISDBT_INCLUDE_DIRS)
