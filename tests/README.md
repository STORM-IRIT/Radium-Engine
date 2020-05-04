TODO: improve documentation

Types of test:
 - unittest: file/function-level testing using Catch2
 - ExampleApps: Dummy applications linking with Radium libraries. Tests the buildchain and basic radium features.
 - PluginsAndLibs: show how to configure compilation of plugins and libs, with one plugins that use the other "plugin" lib. This example test has to be compiled independently of Radium lib.

Tests are ran as follow:
 - `make unittests`: compile the unit tests
 - `make check`: compile and run all the tests
 - `make unit`:
 - `make unitall`:
