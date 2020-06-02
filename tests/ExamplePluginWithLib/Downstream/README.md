# Plugin sharing library - Downstream
This example illustrates how to use a library shipped from another plugin (should be installed first).

## Configure
`cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/  -DQt5_DIR=/pathToInstalledQt5/lib/cmake/Qt5
-DDummyLibrary=/pathToInstalledDummyLibrary ..`

## Compile
```bash
make
```
The plugin can be loaded from the build tree.

## Install
```bash
make install
```
The plugin can be loaded from the install tree.
