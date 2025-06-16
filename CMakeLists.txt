cmake_minimum_required(VERSION 3.30)
project(awCore)

option(AWCORE_BUILD_TESTS "Whether to build tests" ON)
option(AWCORE_BUILD_AWPK "Whether to build awpk packer app" ON)

add_library(awCore STATIC)
add_library(aw::Core ALIAS awCore)
target_compile_features(awCore PUBLIC cxx_std_23)

file(GLOB_RECURSE coreFiles include/*.h include/*.inl src/aw/*.h src/aw/*.cpp src/aw/*.inl LIST_DIRECTORIES ON)
target_sources(awCore PRIVATE ${coreFiles})

add_subdirectory(dependencies)
target_include_directories(awCore PUBLIC include PRIVATE src)
target_link_libraries(awCore PUBLIC nlohmann_json)

if (AWCORE_BUILD_AWPK)
    add_subdirectory(src/awpk)
endif ()

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