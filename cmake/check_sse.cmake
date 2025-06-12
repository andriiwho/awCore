include(CheckCXXCompilerFlag)

# Check AVX
if (NOT MSVC)
    check_cxx_compiler_flag("-mavx" AW_HAS_AVX)
    check_cxx_compiler_flag("-mavx2" AW_HAS_AVX2)

    if (APPLE)
        check_cxx_compiler_flag("-msse" AW_HAS_SSE)
        check_cxx_compiler_flag("-msse2" AW_HAS_SSE2)
        check_cxx_compiler_flag("-mmmx" AW_HAS_SSE_MMX)
    endif ()
elseif (MSVC)
    check_cxx_compiler_flag("/arch:AVX" AW_HAS_AVX)
    check_cxx_compiler_flag("/arch:AVX2" AW_HAS_AV2)
endif ()

if (NOT APPLE)
    cmake_host_system_information(RESULT AW_HAS_SSE QUERY HAS_SSE)
    cmake_host_system_information(RESULT AW_HAS_SSE2 QUERY HAS_SSE2)
    cmake_host_system_information(RESULT AW_HAS_SSE_FP QUERY HAS_SSE_FP)
    cmake_host_system_information(RESULT AW_HAS_SSE_MMX QUERY HAS_SSE_MMX)
endif ()

# Create the directory and configure the file
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core/misc)
configure_file(src/aw/core/aw_core_checks.h.in ${CMAKE_CURRENT_BINARY_DIR}/generated/aw/core/misc/aw_core_checks.h)
target_include_directories(awCore PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/generated)