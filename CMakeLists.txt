cmake_minimum_required (VERSION 3.10)
project (SymmetricStrength)

include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
  GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set (source_dir "${PROJECT_SOURCE_DIR}/src/")


file (GLOB source_files "${source_dir}/*.cpp")
add_executable (SymmetricStrength ${source_files})

target_link_libraries(SymmetricStrength ${CURSES_LIBRARIES} nlohmann_json::nlohmann_json)