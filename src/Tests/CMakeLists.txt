set(target radiumTestsUtils)

file(GLOB sources *.cpp)
file(GLOB headers *.hpp)
file(GLOB inlines *.inl)

add_library          ( ${target} ${sources} ${headers} ${inlines} )
target_link_libraries( ${target} radiumCore )
add_dependencies     ( ${target} radiumCore )


add_subdirectory(CoreTests)
