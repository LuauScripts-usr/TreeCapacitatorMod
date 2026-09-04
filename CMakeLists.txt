cmake_minimum_required(VERSION 3.22)
project(TreeCapacitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set JNI include path directly (for Android NDK-style builds)
set(JAVA_INCLUDE_PATH "/usr/lib/jvm/java-17-openjdk-amd64/include")
set(JAVA_INCLUDE_PATH2 "/usr/lib/jvm/java-17-openjdk-amd64/include/linux")

add_library(tree_capacitor SHARED
    src/Main.cpp
    src/tree_capacitor.cpp
)

target_include_directories(tree_capacitor PRIVATE
    src
    ${JAVA_INCLUDE_PATH}
    ${JAVA_INCLUDE_PATH2}
    android_stubs
)

target_compile_definitions(tree_capacitor PRIVATE
    MC_VERSION_MAJOR=1
    MC_VERSION_MINOR=26
    MC_VERSION_PATCH=33
    MC_VERSION_BUILD=1
)

target_link_libraries(tree_capacitor PRIVATE
    dl
)

set_target_properties(tree_capacitor PROPERTIES
    LINK_FLAGS "-Wl,--gc-sections"
)
