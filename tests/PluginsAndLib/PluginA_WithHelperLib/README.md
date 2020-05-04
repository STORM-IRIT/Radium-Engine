# Plugin with helper lib demo
## Configure
`cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/  -DQt5_DIR=/pathToInstalledQt5/lib/cmake/Qt5 ..`

## Compile
`make` 

## Use
Load the plugin into any plugin compatible Radium app and enjoy the message

## Make the Library usable by client
This will install the library and its cmake configuration scripts in the `CMAKE_INSTALL_PREFIX` directory.
`make install`

