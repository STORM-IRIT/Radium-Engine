set(target coretests)

# All core tests follow the pattern:
# radium_add_test( TARGET core_algebra SRC src/algebra.cpp LIBS radiumCore )
#
function (radium_add_core_test name)
    radium_add_test( TARGET core_${name} SRC src/${name}.cpp LIBS radiumCore )
endfunction()

radium_add_core_test( algebra    )
radium_add_core_test( animation  )
radium_add_core_test( color      )
radium_add_core_test( containers )
radium_add_core_test( distance   )
radium_add_core_test( geometry   )
radium_add_core_test( indexmap   )
radium_add_core_test( mesh       )
radium_add_core_test( raycast    )
radium_add_core_test( string     )
radium_add_core_test( topomesh   )
radium_add_core_test( polyline   )
radium_add_core_test( observer   )

