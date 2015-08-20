cmake_minimum_required (VERSION 2.8)
project (Resizer)

set (CMAKE_C_COMPILER "gcc")
set (CMAKE_C_FLAGS "-g -std=c99 -Wall -Werror -fPIC")

set (ROOT_DIR "${PROJECT_SOURCE_DIR}")
set (PNG_LIBS "-lpng12")
set (IMAGES_DIR "${ROOT_DIR}/_images")

include_directories ("${ROOT_DIR}/api")

add_subdirectory (src)
add_subdirectory (unittest)
add_subdirectory (functest)
# edit the source files not to depend on relative paths to get_images/
