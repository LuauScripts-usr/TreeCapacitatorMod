cmake_minimum_required(VERSION 3.22)
project(TreeCapacitor CXX)

set(CMAKE_CXX_STANDARD 17)

# NDK toolchain is passed via -DCMAKE_TOOLCHAIN_FILE at configure time

add_library(tree_capacitor SHARED
    src/main.cpp
    src/tree_capacitor.cpp
)

target_include_directories(tree_capacitor PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link Dobby
target_link_libraries(tree_capacitor
    ${CMAKE_SOURCE_DIR}/libs/${ANDROID_ABI}/libdobby.so
    log
    dl
)

# Strip for release
set_target_properties(tree_capacitor PROPERTIES
    LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all"
)
