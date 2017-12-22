cmake_minimum_required(VERSION 3.5)

file (GLOB_RECURSE SOURCES "*.c")
file (GLOB_RECURSE HEADERS "../include/fat/*.h")

SET(GCC_COVERAGE_COMPILE_FLAGS "-g")

foreach(f IN LISTS SOURCES)
  get_filename_component(b ${f} NAME)
  set_source_files_properties(${f} PROPERTIES
    COMPILE_DEFINITIONS "__FILENAME__=${b}")
endforeach()
foreach(f IN LISTS HEADERS)
  get_filename_component(b ${f} NAME)
  set_source_files_properties(${f} PROPERTIES
    COMPILE_DEFINITIONS "__FILENAME__=${b}")
endforeach()

include_directories(
  ${CMAKE_CURRENT_LIST_DIR}/../include/fat
)
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_executable(fat ${SOURCES} ${HEADERS})