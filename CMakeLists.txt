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

cmake_host_system_information(RESULT AW_HAS_SSE QUERY HAS_SSE)
cmake_host_system_information(RESULT AW_HAS_SSE2 QUERY HAS_SSE2)
cmake_host_system_information(RESULT AW_HAS_SSE_FP QUERY HAS_SSE_FP)
cmake_host_system_information(RESULT AW_HAS_SSE_MMX QUERY HAS_SSE_MMX)

include(cmake/check_sse.cmake)

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core/misc)
configure_file(src/aw/core/aw_core_checks.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core/misc/aw_core_checks.h)
target_include_directories(awCore PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)