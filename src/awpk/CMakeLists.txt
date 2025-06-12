add_executable(awpk main.cpp)
target_link_libraries(awpk PUBLIC aw::Core)

include(FetchContent)
if (NOT TARGET nlohmann_json)
    FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json)
    FetchContent_MakeAvailable(nlohmann_json)
endif ()

target_link_libraries(awpk PUBLIC nlohmann_json)

macro(run_awpk_for_target targetName)
    add_custom_target(${targetName}_awpk ALL)
    add_custom_command(TARGET ${targetName}_awpk POST_BUILD
        COMMAND awpk ARGS -w ${CMAKE_CURRENT_LIST_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    add_dependencies(${targetName}_awpk awpk)
    add_dependencies(${targetName} ${targetName}_awpk)
endmacro()