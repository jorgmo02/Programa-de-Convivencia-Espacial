# Locate Box2D library
# This module defines
# BOX2D_LIBRARY, the name of the library to link against
# BOX2D_FOUND, if false, do not try to link to Box2D
# BOX2D_INCLUDE_DIRS, where to find Box2D headers
#
# Created by Sven-Hendrik Haase. Based on the FindZLIB.cmake module.

IF(BOX2D_INCLUDE_DIR)
  # Already in cache, be silent
  SET(BOX2D_FIND_QUIETLY TRUE)
ENDIF(BOX2D_INCLUDE_DIR)

FIND_PATH(BOX2D_INCLUDE_DIR Box2D/Box2D.h)

SET(BOX2D_NAMES box2d Box2d BOX2D Box2D)
FIND_LIBRARY(BOX2D_LIBRARY NAMES ${BOX2D_NAMES})
MARK_AS_ADVANCED(BOX2D_LIBRARY BOX2D_INCLUDE_DIR)

# Per-recommendation
SET(BOX2D_INCLUDE_DIRS "../../Box2D/include/")
SET(BOX2D_LIBRARIES    "../../Box2D/lib/Debug/")

# handle the QUIETLY and REQUIRED arguments and set BOX2D_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Box2D DEFAULT_MSG BOX2D_LIBRARY BOX2D_INCLUDE_DIR)