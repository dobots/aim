# This file FindMADP.cmake is created at Almende B.V. It is open-source software and part
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
#  MADP_FOUND        - system has MADP installed
#  MADP_INCLUDE_DIRS - the MADP include directories
#  MADP_LIBRARIES    - link these to use MADP
#  MADP_HAS_NAME_LIB - indicate existence of nameserver lib
#  MADP_HAS_MATH_LIB - indicate existence of math lib

# This script uses LIST APPEND which does only exist for cmake >= 2.4

include(LibFindMacros)

# Dependencies (not useful for MADP)
#libfind_package(MADP blah)

# Use pkg-config to get hints about paths (not useful for MADP, just gives warning)
#libfind_pkg_check_modules(MADP_PKGCONF MADP)

# Include dir
FIND_PATH(MADP_INCLUDE_DIR NAMES madp/base/Agent.h
   PATHS
      ${MADP_PKGCONF_INCLUDE_DIRS}
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /sw # Fink
      /opt/local # DarwinPorts
      /opt/csw # Blastwave
      /opt
      /usr/freeware/include
   PATH_SUFFIXES
      include
)

# Finally find the directory for the library/libraries itself
# See: http://www.cmake.org/cmake/help/cmake2.6docs.html#command:find_path
FIND_LIBRARY(MADP_LIB_Parser NAMES MADPParser 
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_Base NAMES MADPBase 
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_Planning NAMES MADPPlanning
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_Support NAMES MADPSupport
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_POMDP NAMES POMDPSolve
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_LPSolveOLD NAMES LPSolveOld
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_mdp NAMES mdp
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_DAI NAMES DAI
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
      /usr/local
      /usr
   PATH_SUFFIXES
      Build/lib
      Debug/lib
      Release/lib
      lib  
      lib64
)

FIND_LIBRARY(MADP_LIB_DAIProfile NAMES DAIProfile
   PATHS
      ${MADP_PKGCONF_LIBRARY_DIRS}
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
SET(MADP_LIBRARIES
  ${MADP_LIB_Planning}
  ${MADP_LIB_POMDP}
  ${MADP_LIB_LPSolveOLD}
  ${MADP_LIB_mdp}
  ${MADP_LIB_DAI}
  ${MADP_LIB_DAIProfile}
  ${MADP_LIB_Base}
  ${MADP_LIB_Support}
  ${MADP_LIB_Parser}
)

# It could also have been done optionally
#SET(MADP_HAS_DAI_LIB FALSE)
#IF(MADP_LIB_DAI) 
#  LIST(APPEND MADP_LIBRARIES ${MADP_LIB_DAI} )
#  SET(MADP_HAS_DAI_LIB TRUE)  
#ENDIF()

SET(MADP_INCLUDE_DIRS 
   ${MADP_INCLUDE_DIR}/madp
   ${MADP_INCLUDE_DIR}/madp/base
   ${MADP_INCLUDE_DIR}/madp/support
   ${MADP_INCLUDE_DIR}/madp/planning
)

# Debug info
MESSAGE("MADP_INCLUDE DIRS = ${MADP_INCLUDE_DIR}")
MESSAGE("MADP_LIBRARIES = ${MADP_LIBRARIES}")

# Set the include dir variables and the libraries and let libfind_process do the rest.
# NOTE: Singular variables for this library, plural for libraries that this lib depends on.
#set(MADP_PROCESS_INCLUDES MADP_INCLUDE_DIR MADP_INCLUDE_DIRS)
#set(MADP_PROCESS_LIBS MADP_LIBRARY MADP_LIBRARIES)
#libfind_process(MADP)

SET(MADP_FOUND FALSE)
IF(MADP_INCLUDE_DIR AND MADP_LIBRARIES)
    SET(MADP_FOUND TRUE)
ENDIF()

