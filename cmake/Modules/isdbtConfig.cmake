INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ISDBT isdbt)

FIND_PATH(
    ISDBT_INCLUDE_DIRS
    NAMES isdbt/api.h
    HINTS $ENV{ISDBT_DIR}/include
        ${PC_ISDBT_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ISDBT_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/isdbtTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ISDBT DEFAULT_MSG ISDBT_LIBRARIES ISDBT_INCLUDE_DIRS)
MARK_AS_ADVANCED(ISDBT_LIBRARIES ISDBT_INCLUDE_DIRS)
