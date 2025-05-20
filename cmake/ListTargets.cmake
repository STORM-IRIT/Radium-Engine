# https://stackoverflow.com/questions/37434946/how-do-i-iterate-over-all-cmake-targets-programmatically
function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

    foreach(target ${current_targets})
        get_target_property(type ${target} TYPE)
        if(${type} STREQUAL "EXECUTABLE" OR ${type} STREQUAL "SHARED_LIBRARY")
            list(APPEND ${targets} ${target})
        endif()
    endforeach()
endmacro()
