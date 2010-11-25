# - Try to find the Exiv2 library
#
#  EXIV2_MIN_VERSION - You can set this variable to the minimum version you need 
#                      before doing FIND_PACKAGE(Exiv2). The default is 0.12.
# 
# Once done this will define
#
#  EXIV2_FOUND - system has libexiv2
#  EXIV2_INCLUDE_DIR - the libexiv2 include directory
#  EXIV2_LIBRARIES - Link these to use libexiv2
#  EXIV2_DEFINITIONS - Compiler switches required for using libexiv2
#

# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

  # in cache already
  set(EXIV2_FOUND TRUE)

else (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
  if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    if(NOT EXIV2_MIN_VERSION)
      set(EXIV2_MIN_VERSION "0.12")
    endif(NOT EXIV2_MIN_VERSION)


    find_package(PkgConfig)
    pkg_check_modules(PC_EXIV2 QUIET exiv2>=${EXIV2_MIN_VERSION})

    set(EXIV2_FOUND PC_EXIV2_FOUND)
    set(EXIV2_VERSION_GOOD_FOUND PC_EXIV2_FOUND)

	if (EXIV2_DESIRED_VERSION)
		pkg_check_modules(PC_EXIV2_DESIRED QUIET exiv2>=${EXIV2_DESIRED_VERSION})
		set(EXIV2_DESIRED_VERSION_FOUND PC_EXIV2_DESIRED_FOUND)
	endif (EXIV2_DESIRED_VERSION)

  else(NOT WIN32)
     #Better check
     set(EXIV2_VERSION_GOOD_FOUND TRUE)
	 set(EXIV2_DESIRED_VERSION_FOUND TRUE)
  endif (NOT WIN32)

  if(EXIV2_VERSION_GOOD_FOUND)
     set(EXIV2_DEFINITIONS ${PC_EXIV2_CFLAGS})
 
     find_path(EXIV2_INCLUDE_DIR
       NAMES exiv2/exif.hpp
       HINTS ${PC_EXIV2_INCLUDEDIR}
     )

     find_library(EXIV2_LIBRARIES
       NAMES exiv2 libexiv2
       HINTS ${PC_EXIV2_LIBDIR}
     )
  
     if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
        set(EXIV2_FOUND TRUE)
        # TODO version check is missing
     endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
   endif(EXIV2_VERSION_GOOD_FOUND)
   if (EXIV2_FOUND)
      if (NOT Exiv2_FIND_QUIETLY)
       message(STATUS "Found Exiv2: ${EXIV2_LIBRARIES}")
      endif (NOT Exiv2_FIND_QUIETLY)
   else (EXIV2_FOUND)
     if (Exiv2_FIND_REQUIRED)
       if (NOT EXIV2_INCLUDE_DIR)
         message(FATAL_ERROR "Could NOT find Exiv2 header files")
       endif (NOT EXIV2_INCLUDE_DIR)
       if (NOT EXIV2_LIBRARIES)
           message(FATAL_ERROR "Could NOT find Exiv2 library")
       endif (NOT EXIV2_LIBRARIES)
     endif (Exiv2_FIND_REQUIRED)
   endif (EXIV2_FOUND)

  mark_as_advanced(EXIV2_INCLUDE_DIR EXIV2_LIBRARIES)
  
endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

