# Plugin sharing library - Downstream
This example illustrates how to ship a library with a Radium plugin, and make the library available
to other plugins.

## Configure and compile
```bash
mkdir build
ce build
cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/  -DQt5_DIR=/pathToInstalledQt5/lib/cmake/Qt5 ..`
```

## Compile
```bash
make
```
The plugin can be loaded from the build tree.

## Make the Library usable by client
This will install the library and its cmake configuration scripts in the `CMAKE_INSTALL_PREFIX` directory.
```bash
make install
```

