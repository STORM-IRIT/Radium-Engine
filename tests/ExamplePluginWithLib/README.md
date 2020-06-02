# Plugin sharing library
This example illustrate how to distribute a library alongside a plugin Radium, while making the
library available for any other plugin.

It has two components:
 - Upstream: Plugin shipped with library,
 - Downstream: User plugin using the library provided by Upstream.

## Configure and compile
This example can either be compiled with Radium, or separately. For the latter, Radium must be
already compiled and installed.

```bash
mkdir build
cd build
cmake -DRadium_DIR=/pathToInstalledRadium/lib/cmake/Radium/ ../
make install
```

## Use
Load the Upstream and Downstream plugins into any plugin compatible Radium app, a confirmation
message should be printed to standard output.

