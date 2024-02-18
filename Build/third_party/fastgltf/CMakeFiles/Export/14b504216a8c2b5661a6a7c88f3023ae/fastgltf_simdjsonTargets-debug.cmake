#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "fastgltf::fastgltf_simdjson" for configuration "Debug"
set_property(TARGET fastgltf::fastgltf_simdjson APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(fastgltf::fastgltf_simdjson PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/lib/fastgltf_simdjson.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/bin/fastgltf_simdjson.dll"
  )

list(APPEND _cmake_import_check_targets fastgltf::fastgltf_simdjson )
list(APPEND _cmake_import_check_files_for_fastgltf::fastgltf_simdjson "${_IMPORT_PREFIX}/lib/fastgltf_simdjson.lib" "${_IMPORT_PREFIX}/bin/fastgltf_simdjson.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
