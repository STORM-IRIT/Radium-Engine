\page develWithRadium How to contribute to Radium Libraries
[TOC]

# Radium libraries
Direct contributions to master are closed.
Please submit your pull request.

# Radium test suite
## Preliminaries
Radium testing is done using two tools:
-   CTest: the test system by CMake. It provides compilation, running and basic reporting mechanisms for tests.
-   Continuous Integration system: Travis.ci and AppVeyor. We use these services to call CTest on several platforms (windows, ubuntu and MacOs) with several compilers, and validate any change made to the main repository.

As a Radium contributor, and in most cases, you will have to work only with CTest, and let the CI system run your tests automatically.
In this section we describe how to write a new test, how to insert it in the testing suite, and how to validate your results on your computer and the CI systems.

## How to write tests

There is two kind of tests, integration test and unit test. Tests are run with the `check` target and it's variant (`check_verbose`, `test`, `run_unittests`, `run_unittests_verbose`)

* integration test are based on program return values, using the ctest suite. The tests are set in `tests/integration/CMakeLists.txt`, see `add_test` [documentation](https://cmake.org/cmake/help/latest/command/add_test.html).
  Integration tests typically run some program, catch and analyze the output, compare to reference output etc.
* unit test use Catch2 framework [see tutorial](https://github.com/catchorg/Catch2/blob/devel/docs/tutorial.md)

Note that we target Unit Tests, which can be defined as:
> The purpose of a unit test in software engineering is to verify the behavior of a relatively small piece of software,
> independently from other parts. Unit tests are narrow in scope, and allow us to cover all cases, ensuring that every
> single part works correctly.
>
> Source: [https://www.toptal.com/qa/how-to-write-testable-code-and-why-it-matters](https://www.toptal.com/qa/how-to-write-testable-code-and-why-it-matters)

To add a unit test, 
create the test file in tests/unittest/LIB/mytestfile.cpp where LIB correspond to the library you are testing (Core, Engine, Gui, IO).
Edit `tests/unittest/CMakeLists.txt` and add your test to the list of sources of `add_executable(unittests ...`.
If your test needs data, add them to `tests/unittest/data`. unittest working dir is `tests/unittest` so you can refer your data with a relative path starting with `data`. No output file are allowed in unittest.
`TEST_CASE` first argument is the unique test name, second argument is a list of optional tags. You can add independent `TEST_SECTION`s to your test, please refer to [catch2 documentation](https://github.com/catchorg/Catch2/tree/devel/docs).

Here is an example:

\snippet unittest/Core/obb.cpp obb test
