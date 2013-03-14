# This file FindYARP.cmake is created at Almende B.V. It is open-source software and part
# of the Common Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools.
# Ranging from thread pools, and TCP/IP components to control architectures and learning
# algorithms. This software is published under the GNU Lesser General Public license, under
# version 3 (or later). You can find a version of the license on Debian (derived) systems at
# /usr/share/common-licenses/LGPL-3.

# It is not possible to add usage restrictions to a free open-source license. Nevertheless, we
# personally strongly object against this software used by the military, in the bio-industry, 
# for animal experimentation, or anything that violates the Universal Declaration of Human 
# Rights. This is, again, no usage restriction, it is a non-binding recommendation.

# Copyright © 2010 Anne van Rossum

# This will define:
#
#  YARP_FOUND        - system has YARP installed
#  YARP_INCLUDE_DIRS - the YARP include directories
#  YARP_LIBRARIES    - link these to use YARP
#  YARP_HAS_NAME_LIB - indicate existence of nameserver lib
#  YARP_HAS_MATH_LIB - indicate existence of math lib

# This script uses LIST APPEND which does only exist for cmake >= 2.4

include(LibFindMacros)

# Dependencies (not useful for YARP)
#libfind_package(YARP blah)

# Use pkg-config to get hints about paths (not useful for YARP, just gives warning)
#libfind_pkg_check_modules(YARP_PKGCONF YARP)

# Include dir
FIND_PATH(YARP_INCLUDE_DIR NAMES yarp/os/Bottle.h
   PATHS
      ${YARP_PKGCONF_INCLUDE_DIRS}
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /sw # Fink
      /opt/local # DarwinPorts
      /opt/csw # Blastwave
      /opt
      /usr/freeware
   PATH_SUFFIXES
      include
)

# Finally find the directory for the library/libraries itself
# See: http://www.cmake.org/cmake/help/cmake2.6docs.html#command:find_path
FIND_LIBRARY(YARP_LIB_OS
   NAMES 
      YARP_OS 
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(YARP_LIB_sig NAMES YARP_sig 
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(YARP_LIB_dev NAMES YARP_dev
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(YARP_LIB_init NAMES YARP_init
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(YARP_LIB_name NAMES YARP_name
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(YARP_LIB_math NAMES YARP_math
   PATHS
      ${YARP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

# Set the libraries
SET(YARP_LIBRARIES
  ${YARP_LIB_OS}
  ${YARP_LIB_dev}
  ${YARP_LIB_sig}
  ${YARP_LIB_init}
)

SET(YARP_HAS_NAME_LIB FALSE)
IF(YARP_LIB_name) 
  LIST(APPEND YARP_LIBRARIES ${YARP_LIB_name} )
  SET(YARP_HAS_NAME_LIB TRUE)  
ENDIF()

SET(YARP_HAS_MATH_LIB FALSE)
IF(YARP_LIB_math) 
  LIST(APPEND YARP_LIBRARIES ${YARP_LIB_math} )
  SET(YARP_HAS_MATH_LIB TRUE)  
ENDIF()

# Debug info
#MESSAGE("${YARP_INCLUDE_DIR}")
#MESSAGE("${YARP_LIBRARIES}")

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries that this lib depends on.
#set(YARP_PROCESS_INCLUDES YARP_INCLUDE_DIR YARP_INCLUDE_DIRS)
#set(YARP_PROCESS_LIBS YARP_LIBRARY YARP_LIBRARIES)
#libfind_process(YARP)

SET(YARP_FOUND FALSE)
IF(YARP_INCLUDE_DIR AND YARP_LIBRARIES)
    SET(YARP_FOUND TRUE)
ENDIF()

