cmake_minimum_required(VERSION 3.10)
project(CheckAVXSupport CXX)

include(CheckCXXCompilerFlag)

# Check AVX
if (NOT MSVC)
    check_cxx_compiler_flag("-mavx" AW_HAS_AVX)
    check_cxx_compiler_flag("-mavx2" AW_HAS_AVX2)
elseif (MSVC)
    check_cxx_compiler_flag("/arch:AVX" AW_HAS_AVX)
    check_cxx_compiler_flag("/arch:AVX2" AW_HAS_AV2)
endif ()