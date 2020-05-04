# Plugin sharing library
This example requires that Radium is already compiled and installed.

## Configure and compile

```bash
mkdir build
cd build
cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/ ../
make install
```

## Use
Load the plugins into any plugin compatible Radium app and enjoy the message
## Install
This will install the demo plugins  and the associated library (with its cmake configuration scripts) in the `CMAKE_INSTALL_PREFIX` directory.
`make install`

