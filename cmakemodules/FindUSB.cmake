# - Try to find the freetype library
# Once done this defines
#
#  LIBUSB_FOUND - system has libusb
#  LIBUSB_INCLUDE_DIR - the libusb include directory
#  LIBUSB_LIBRARIES - Link these to use libusb
#
# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)

  # in cache already
  set(LIBUSB_FOUND TRUE)

else (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
  IF (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    INCLUDE(UsePkgConfig)

    PKGCONFIG(libusb _libUSBIncDir _libUSBLinkDir _libUSBLinkFlags _libUSBCflags)
  ENDIF(NOT WIN32)

  FIND_PATH(LIBUSB_INCLUDE_DIR usb.h )

  FIND_LIBRARY(LIBUSB_LIBRARY NAMES usb 
    PATHS ${_libUSBLinkDir} )

  set( LIBUSB_LIBRARIES ${LIBUSB_LIBRARY} CACHE INTERNAL "The libraries for libusb" )

  if (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
     set( LIBUSB_FOUND TRUE)
  endif (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)

  if (LIBUSB_FOUND)
    if (NOT USB_FIND_QUIETLY)
      message(STATUS "Found LIBUSB: ${LIBUSB_LIBRARIES}")
    endif (NOT USB_FIND_QUIETLY)
  else (LIBUSB_FOUND)
    if (USB_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find LIBUSB")
    endif (USB_FIND_REQUIRED)
  endif (LIBUSB_FOUND)

  MARK_AS_ADVANCED(
     LIBUSB_INCLUDE_DIR LIBUSB_LIBRARIES 
  )

endif (LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
