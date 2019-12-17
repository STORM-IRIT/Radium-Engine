TODO: improve documentation

Types of test:
 - unittest: file/function-level testing using Catch2
 - ExampleApp: Dummy application linking with Radium libraries. Tests the buildchain.

Tests are ran as follow:
 - `make unittest`: compile and run the unit tests
 - `make run`: compile and run the ExampleApps
 - `make check`: compile and run all the tests
