# Plugin sharing library - Downstream
This example illustrates how to use a library shipped from another plugin (should be installed first).

## Configure
`cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/ -DExampleLibraryUpstream_DIR=/pathToInstalledUpstreamLibrary ..`

If Qt5 package is not found, add the option `-DQt5_DIR=/pathToInstalledQt5/lib/cmake/Qt5` to the cmake command.

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
