# Usefull helper for radium compilation
function (radium_cotire)
  if(${RADIUM_USE_COTIRE})
    cotire(${ARGV})
  endif()
endfunction()
