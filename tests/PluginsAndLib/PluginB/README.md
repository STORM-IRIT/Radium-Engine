# Plugin using external lib demo
## Configure
`cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/  -DQt5_DIR=/pathToInstalledQt5/lib/cmake/Qt5 
-DDummyLibrary=/pathToInstalledDummyLibrary ..`

## Compile
`make` 

## Install
`make install` 

## Use
Load the plugin, from its builtree or installation tree into any plugin compatible Radium app and enjoy the message
