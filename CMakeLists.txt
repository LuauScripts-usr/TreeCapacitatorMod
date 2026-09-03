cmake_minimum_required(VERSION 3.22)
project(TreeCapacitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED DOBBY_INCLUDE_DIR OR NOT DEFINED DOBBY_LIBRARY)
  message(FATAL_ERROR "Pass -DDOBBY_INCLUDE_DIR=... and -DDOBBY_LIBRARY=...")
endif()

add_library(tree_capacitor SHARED
    src/Main.cpp
    src/tree_capacitor.cpp
)

target_include_directories(tree_capacitor PRIVATE
    src
    ${DOBBY_INCLUDE_DIR}
)

target_compile_definitions(tree_capacitor PRIVATE
    MC_VERSION_MAJOR=1
    MC_VERSION_MINOR=26
    MC_VERSION_PATCH=33
    MC_VERSION_BUILD=1
)

target_link_libraries(tree_capacitor PRIVATE
    ${DOBBY_LIBRARY}
    log
    dl
)

set_target_properties(tree_capacitor PROPERTIES
    LINK_FLAGS "-Wl,--gc-sections"
)
