add_executable(awpk main.cpp)
target_link_libraries(awpk PUBLIC aw::Core)

macro(run_awpk_for_target targetName)
    add_custom_target(${targetName}_awpk ALL)
    add_custom_command(TARGET ${targetName}_awpk POST_BUILD
        COMMAND awpk ARGS -w ${CMAKE_CURRENT_LIST_DIR}
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
    add_dependencies(${targetName}_awpk awpk)
    add_dependencies(${targetName} ${targetName}_awpk)
endmacro()