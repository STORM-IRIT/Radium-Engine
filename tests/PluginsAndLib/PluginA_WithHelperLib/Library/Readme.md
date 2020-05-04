# Dummy Library exemple
## usage into DummyPlugin
Will be configured, compiled and installed alongside the DummyPlugin when configuring and installing the DummyPlugin.

## autonomous usage
configure with
`cmake -DRadium_DIR=/PathToInstalledRadium/lib/cmake/Radium/ ..`
compile and install using `make install`

To use the library, just do `FindPackage(DummyLibrary)` then `target_link_libraries(yourTarget DummyLibrary::DummyLibrary)` in your client application or lib and configure with `-D/pathToInstalledDummyLibrary` 
