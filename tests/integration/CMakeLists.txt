
add_custom_target(integration)

# this target generates all references files with HEAD version of the source code, use it when updating the tests while not updating the libs
add_custom_target(integration_create_ref)

# read tinyply tests
add_executable(tinyply-info
    tinyply-info/main.cpp)
target_link_libraries(tinyply-info PUBLIC Core IO)
configure_radium_app(NAME tinyply-info)
addRunAndDebugTargets(tinyply-info)


add_custom_target(tinyply-info_create_ref)
add_dependencies(integration_create_ref tinyply-info_create_ref)

add_custom_command(TARGET tinyply-info_create_ref 
    PRE_BUILD  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tinyply-info/
    COMMAND bash -c "$<TARGET_FILE:tinyply-info>  2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_SOURCE_DIR}/data/references/tinyply-info.txt"
    COMMAND_EXPAND_LISTS
    VERBATIM
    COMMENT "generating references for tinyply-info integration test"
    DEPENDS tinyply-info)

add_test(NAME tinyply-info_create 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tinyply-info/
    COMMAND bash -c "$<TARGET_FILE:tinyply-info> 2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_BINARY_DIR}/output.txt")
 
add_test(NAME tinyply-info_compare
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tinyply-info/
    COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_BINARY_DIR}/output.txt ${CMAKE_CURRENT_SOURCE_DIR}/data/references/tinyply-info.txt)

add_dependencies(integration tinyply-info)
