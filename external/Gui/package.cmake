# WARNING : if some paths are changed in the following variables, think to change it also in CMakeLists.txt
if(NOT DEFINED PowerSlider_DIR)
    set(PowerSlider_sub_DIR lib/cmake/PowerSlider/ CACHE INTERNAL "")
    set(PowerSlider_DIR ${CMAKE_INSTALL_PREFIX}/${PowerSlider_sub_DIR})
endif()
