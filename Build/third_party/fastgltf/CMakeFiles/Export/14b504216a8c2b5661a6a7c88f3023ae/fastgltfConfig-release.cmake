#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fastgltf::fastgltf" for configuration "Release"
set_property(TARGET fastgltf::fastgltf APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(fastgltf::fastgltf PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/fastgltf.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "fastgltf::fastgltf_simdjson"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/fastgltf.dll"
  )

list(APPEND _cmake_import_check_targets fastgltf::fastgltf )
list(APPEND _cmake_import_check_files_for_fastgltf::fastgltf "${_IMPORT_PREFIX}/lib/fastgltf.lib" "${_IMPORT_PREFIX}/bin/fastgltf.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
