
add_custom_target(integration)

# this target generates all references files with HEAD version of the source code, use it when updating the tests while not updating the libs
add_custom_target(integration_create_ref)

set(INTEGRATION_TEST TinyPlyFileLoader)
# TinyPlyFileLoader read tests
add_executable(integration_${INTEGRATION_TEST}
    ${INTEGRATION_TEST}/main.cpp)
target_link_libraries(integration_${INTEGRATION_TEST} PUBLIC Core IO)
configure_radium_app(NAME integration_${INTEGRATION_TEST})
addRunAndDebugTargets(integration_${INTEGRATION_TEST})

add_custom_target(integration_${INTEGRATION_TEST}_create_ref)
add_dependencies(integration_create_ref integration_${INTEGRATION_TEST}_create_ref)

add_custom_command(TARGET integration_${INTEGRATION_TEST}_create_ref 
    PRE_BUILD  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${INTEGRATION_TEST}/
    COMMAND bash -c "$<TARGET_FILE:integration_${INTEGRATION_TEST}>  2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_SOURCE_DIR}/data/references/${INTEGRATION_TEST}.txt"
    COMMAND_EXPAND_LISTS
    VERBATIM
    COMMENT "generating references for integration_${INTEGRATION_TEST} integration test"
    DEPENDS integration_${INTEGRATION_TEST})

add_test(NAME integration_${INTEGRATION_TEST}_save_output 
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${INTEGRATION_TEST}/
    COMMAND bash -c "$<TARGET_FILE:integration_${INTEGRATION_TEST}> 2>&1 | cut -f4- -d: | grep -v \"Loading Time\" > ${CMAKE_CURRENT_BINARY_DIR}/output.txt")
 
add_test(NAME integration_${INTEGRATION_TEST}_compare_to_ref
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${INTEGRATION_TEST}/
    COMMAND ${CMAKE_COMMAND} -E compare_files ${CMAKE_CURRENT_BINARY_DIR}/output.txt ${CMAKE_CURRENT_SOURCE_DIR}/data/references/${INTEGRATION_TEST}.txt)

add_dependencies(integration integration_${INTEGRATION_TEST})
