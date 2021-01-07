
add_custom_target(integration)

# this target generates all references files with HEAD version of the source code, use it when updating the tests while not updating the libs
add_custom_target(integration_create_ref)

set(INTEGRATION_TARGET TinyPlyFileLoader)
# TinyPlyFileLoader read tests
add_executable(integration_TinyPlyFileLoader
    TinyPlyFileLoader/main.cpp)
target_link_libraries(integration_TinyPlyFileLoader PUBLIC Core IO)
configure_radium_app(NAME integration_TinyPlyFileLoader)
addRunAndDebugTargets(integration_TinyPlyFileLoader)

add_custom_target(integration_TinyPlyFileLoader_create_ref)
add_dependencies(integration_create_ref integration_TinyPlyFileLoader_create_ref)

add_custom_command(TARGET integration_TinyPlyFileLoader_create_ref 
    PRE_BUILD  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TinyPlyFileLoader/
    COMMAND bash -c "$<TARGET_FILE:integration_TinyPlyFileLoader>  2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_SOURCE_DIR}/data/references/TinyPlyFileLoader.txt"
    COMMAND_EXPAND_LISTS
    VERBATIM
    COMMENT "generating references for integration_TinyPlyFileLoader integration test"
    DEPENDS integration_TinyPlyFileLoader)

add_test(NAME integration_TinyPlyFileLoader_save_output 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TinyPlyFileLoader/
    COMMAND bash -c "$<TARGET_FILE:integration_TinyPlyFileLoader> 2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_BINARY_DIR}/output.txt")
 
add_test(NAME integration_TinyPlyFileLoader_compare_to_ref
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/TinyPlyFileLoader/
    COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_BINARY_DIR}/output.txt ${CMAKE_CURRENT_SOURCE_DIR}/data/references/TinyPlyFileLoader.txt)

add_dependencies(integration integration_TinyPlyFileLoader)
