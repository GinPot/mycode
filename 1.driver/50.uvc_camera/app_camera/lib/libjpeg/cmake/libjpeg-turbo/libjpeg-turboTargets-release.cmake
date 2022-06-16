#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "libjpeg-turbo::jpeg" for configuration "Release"
set_property(TARGET libjpeg-turbo::jpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo::jpeg PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libjpeg.so.8.2.2"
  IMPORTED_SONAME_RELEASE "libjpeg.so.8"
  )

list(APPEND _IMPORT_CHECK_TARGETS libjpeg-turbo::jpeg )
list(APPEND _IMPORT_CHECK_FILES_FOR_libjpeg-turbo::jpeg "${_IMPORT_PREFIX}/lib/libjpeg.so.8.2.2" )

# Import target "libjpeg-turbo::turbojpeg" for configuration "Release"
set_property(TARGET libjpeg-turbo::turbojpeg APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo::turbojpeg PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libturbojpeg.so.0.2.0"
  IMPORTED_SONAME_RELEASE "libturbojpeg.so.0"
  )

list(APPEND _IMPORT_CHECK_TARGETS libjpeg-turbo::turbojpeg )
list(APPEND _IMPORT_CHECK_FILES_FOR_libjpeg-turbo::turbojpeg "${_IMPORT_PREFIX}/lib/libturbojpeg.so.0.2.0" )

# Import target "libjpeg-turbo::turbojpeg-static" for configuration "Release"
set_property(TARGET libjpeg-turbo::turbojpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo::turbojpeg-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "ASM;C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libturbojpeg.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libjpeg-turbo::turbojpeg-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libjpeg-turbo::turbojpeg-static "${_IMPORT_PREFIX}/lib/libturbojpeg.a" )

# Import target "libjpeg-turbo::jpeg-static" for configuration "Release"
set_property(TARGET libjpeg-turbo::jpeg-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(libjpeg-turbo::jpeg-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "ASM;C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libjpeg.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS libjpeg-turbo::jpeg-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_libjpeg-turbo::jpeg-static "${_IMPORT_PREFIX}/lib/libjpeg.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
