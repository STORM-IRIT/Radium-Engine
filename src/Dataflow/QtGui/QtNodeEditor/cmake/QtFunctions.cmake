cmake_minimum_required(VERSION 3.12)

# Find Qt5 or Qt6 packages Parameters: COMPONENTS <component_list>: optional parameter listing the
# Qt packages (e.g. Core, Widgets REQUIRED: optional parameter propagated to find_package
#
# Usage: find_qt_package(COMPONENTS Core Widgets OpenGL Xml REQUIRED) which is equivalent to:
# find_package(Qt6 COMPONENTS Core Widgets OpenGL Xml REQUIRED) if Qt6 is available, or:
# find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED) otherwise.
#
# Qt5 and Qt6 can be retrieved using versionless targets introduced in Qt5.15:
# https://doc.qt.io/qt-6/cmake-qt5-and-qt6-compatibility.html#versionless-targets
macro(find_qt_package)
    set(options REQUIRED)
    set(oneValueArgs "")
    set(multiValueArgs COMPONENTS)

    cmake_parse_arguments(MY_OPTIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT MY_OPTIONS_COMPONENTS) # User didn't enter any component
        set(MY_OPTIONS_COMPONENTS "")
    endif()

    find_package(Qt6 COMPONENTS ${MY_OPTIONS_COMPONENTS} QUIET)
    if(NOT Qt6_FOUND)
        if(${MY_OPTIONS_REQUIRED})
            find_package(Qt5 5.15 COMPONENTS ${MY_OPTIONS_COMPONENTS} REQUIRED)
        else()
            find_package(Qt5 5.15 COMPONENTS ${MY_OPTIONS_COMPONENTS})
        endif()
    endif()
endmacro()

# Find Qt5 or Qt6 dependency Parameters: COMPONENTS <component_list>: optional parameter listing the
# Qt packages (e.g. Core, Widgets REQUIRED: optional parameter propagated to find_package
#
# Usage example: find_package(Qt5 COMPONENTS Core Widgets OpenGL Xml REQUIRED)
#
# Qt5 and Qt6 can be retrieved using versionless targets introduced in Qt5.15:
# https://doc.qt.io/qt-6/cmake-qt5-and-qt6-compatibility.html#versionless-targets
macro(find_qt_dependency)
    set(options REQUIRED)
    set(oneValueArgs "")
    set(multiValueArgs COMPONENTS)

    cmake_parse_arguments(MY_OPTIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT MY_OPTIONS_COMPONENTS) # User didn't enter any component
        set(MY_OPTIONS_COMPONENTS "")
    endif()

    find_package(Qt6 COMPONENTS ${MY_OPTIONS_COMPONENTS} QUIET)
    if(NOT Qt6_FOUND)
        if(${MY_OPTIONS_REQUIRED})
            find_package(Qt5 5.15 COMPONENTS ${MY_OPTIONS_COMPONENTS} REQUIRED)
        else()
            find_package(Qt5 5.15 COMPONENTS ${MY_OPTIONS_COMPONENTS})
        endif()
    endif()
endmacro()
