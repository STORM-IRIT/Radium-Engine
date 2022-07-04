# PaintyStrokes

```{.bash}
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DRadium_DIR={RADIUM_DIR}/Bundle-GNU/lib/cmake/Radium/ ../
./src/DrawStrokes
```

Disclaimer:
You have to run DrawStrokes from the ./build directory since the Assets directory
to read/load XML files is relative to the execution directory
