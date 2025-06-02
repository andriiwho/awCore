cmake_minimum_required(VERSION 3.30)
project(awCore)

option(AWCORE_BUILD_TESTS "Whether to build tests" ON)

add_library(awCore STATIC)
target_compile_features(awCore PUBLIC cxx_std_23)

file(GLOB_RECURSE coreFiles include/*.h include/*.inl src/*.h src/*.cpp src/*.inl LIST_DIRECTORIES ON)
target_sources(awCore PRIVATE ${coreFiles})

target_include_directories(awCore PUBLIC include PRIVATE src)

if (AWCORE_BUILD_TESTS)
    add_subdirectory(tests)
endif ()