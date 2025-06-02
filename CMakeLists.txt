cmake_minimum_required(VERSION 3.30)
project(awCore)

option(AWCORE_BUILD_TESTS "Whether to build tests" ON)

add_library(awCore STATIC)
add_library(aw::Core ALIAS awCore)
target_compile_features(awCore PUBLIC cxx_std_23)

file(GLOB_RECURSE coreFiles include/*.h include/*.inl src/*.h src/*.cpp src/*.inl LIST_DIRECTORIES ON)
target_sources(awCore PRIVATE ${coreFiles})

target_include_directories(awCore PUBLIC include PRIVATE src)

if (AWCORE_BUILD_TESTS)
    add_subdirectory(tests)
endif ()


include(cmake/check_sse.cmake)

if (AW_HAS_AVX2)
    if (MSVC)
        target_compile_options(awCore PUBLIC "/arch:AVX2")
    else ()
        target_compile_options(awCore PUBLIC "-mavx2")
    endif ()
elseif (AW_HAS_AVX)
    if (MSVC)
        target_compile_options(awCore PUBLIC "/arch:AVX")
    else ()
        target_compile_options(awCore PUBLIC "-mavx")
    endif ()
endif ()