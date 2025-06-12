include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE testFiles *.cpp LIST_DIRECTORIES ON)
add_executable(awCoreTests ${testFiles})

target_link_libraries(awCoreTests PRIVATE awCore GTest::gtest_main)
include(GoogleTest)

gtest_discover_tests(awCoreTests)

# Copy assets to binary dir
if (IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/assets)
    add_custom_command(TARGET awCoreTests POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:awCoreTests>/assets)
    file(GLOB_RECURSE assetFiles assets/*.* LIST_DIRECTORIES ON)
    foreach (assetFile ${assetFiles})
        message(STATUS "Copying file ${assetFile} to tests directory")
        get_filename_component(outFile ${assetFile} NAME)
        add_custom_command(TARGET awCoreTests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${assetFile} $<TARGET_FILE_DIR:awCoreTests>/assets/${outFile})
    endforeach ()
endif ()

if (AWCORE_BUILD_AWPK)
    run_awpk_for_target(awCoreTests)
    add_custom_command(TARGET awCoreTests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_LIST_DIR}/test.awpk $<TARGET_FILE_DIR:awCoreTests>/test.awpk)
endif ()