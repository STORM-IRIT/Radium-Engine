\page develWithRadium How to code in Radium
[TOC]

There are three main options to work with Radium:
1. Write a plugin: full access to the Radium data structures, perfect to implement a new functionality: mesh processing, rendering.
2. Write an application: give total control over the GUI, the camera settings, etc...
3. Contribute to Radium libraries: to improve/extend core components of Radium.
4. Testing: extend radium tests to improve testing coverage or when adding new functionality.

# Radium Plugin
Tutorial and documentation here: [https://github.com/STORM-IRIT/Radium-PluginExample](https://github.com/STORM-IRIT/Radium-PluginExample)

# Radium Application
Tutorial here: [https://github.com/STORM-IRIT/Radium-AppExample](https://github.com/STORM-IRIT/Radium-AppExample)

# Radium libraries
Direct contributions to master are closed.
Please submit your pull request.

# Radium coding style
Please follow the scripts/clang-format coding style (tested `with clang-format 6.0`).
To use it, you have to copy or link `scripts/clang-format` to `.clang-format` (in Radium-Engine root dir).
We also provide a pre commit hook that checks the committed files are correctly formatted.
To install both hooks and clang-format, simply run `./scripts/install-scripts-linux.sh` on linux, or adapt to your OS.

# Radium test suite
## Preliminaries
Radium testing is done using two tools:
-   CTest: the test system by CMake. It provides compilation, running and basic reporting mechanisms for tests.
-   Continuous Integration system: Travis.ci and AppVeyor. We use these services to call CTest on several platforms (windows, ubuntu and MacOs) with several compilers, and validate any change made to the main repository.

As a Radium contributor, and in most cases, you will have to work only with CTest, and let the CI system run your tests automatically.
In this section we describe how to write a new test, how to insert it in the testing suite, and how to validate your results on your computer and the CI systems.

## How to write tests
Tests are C++ executables, expected to set their return value accordingly to the test result, e.g. `return EXIT_FAILURE;` or by calling `abort();` in case of failure.

Their source code is stored in `Radium-Engine/tests/{Core,Engine,IO,Gui}Tests/src`.

We provide in `Radium-Engine/tests/Tests.hpp` some utility functions to ease tests implementation.

Let's consider the following snippet:`

~~~{.cpp}
#include <Core/Math/LinearAlgebra.hpp>                        // include path contains Radium sources,
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh> // Radium's dependencies
#include <Tests.hpp>                                          // and the testing tools

namespace Ra {
namespace Testing {

void run() {
    bool condition = true;
    RA_VERIFY( condition, "This test is passing" );
    // RA_VERIFY( ! condition, "This test does not pass" );
}

} // namespace Testing
} // namespace Ra


int main(int argc, const char **argv) {
   using namespace Ra;

   if(!Testing::init_testing(1, argv))
   {
        return EXIT_FAILURE;
   }

   #pragma omp parallel for
   for(int i = 0; i < Testing::g_repeat; ++i)
   {
       CALL_SUBTEST(( Testing::run() ));
   }

   return EXIT_SUCCESS;
}
~~~

The `Ra::Testing::run()` method is performing the tests, and use the macro `RA_VERIFY` to abort the execution if the condition fails. This macro is provided in `<Tests.hpp>`.

The `main()` is simple is should not vary to much across tests.
It first initializes the testing helpers (including random number generation), and then call the testing function `g_repeat` times.
Repetition is strongly encouraged, especially to test or detect piece of codes involving non-deterministic behavior.
Of course, multiple functions can be called in place of the `run()` function shown in this example. A standard use case is:

~~~{.cpp}
     #pragma omp parallel for
     for(int i = 0; i < Testing::g_repeat; ++i)
     {
          CALL_SUBTEST(( Testing::run<float>() ));
          CALL_SUBTEST(( Testing::run<double>() ));
          // ... other types
     }
~~~

Note that we target Unit Tests, which can be defined as:
> The purpose of a unit test in software engineering is to verify the behavior of a relatively small piece of software,
> independently from other parts. Unit tests are narrow in scope, and allow us to cover all cases, ensuring that every
> single part works correctly.
>
> Source: [https://www.toptal.com/qa/how-to-write-testable-code-and-why-it-matters](https://www.toptal.com/qa/how-to-write-testable-code-and-why-it-matters)

## How to add a test to the testing suite
The testing suite is handled by `cmake`: tests need to be added with their sources and dependencies.
We provide a `cmake` helper function to add a test easily:

~~~cmake
    radium_add_test( TARGET my_test_name SRC src/my_test.cpp LIBS radiumCore radiumEngine )
~~~

See `Radium-Engine/cmake/ConfigureTesting.cmake` for the implementation of this function, and `Radium-Engine/tests/CoreTests/CMakeLists.txt` for an usage example.

Tests are located in `Radium-Engine/tests/{Core,Engine,IO,Gui}Tests/src`. Currently `cmake` scripts are configured only for `Core`, you might need to copy and adapt them in case you add tests for the others packages (this is what you should do ! ;) ).

## How to run tests locally and on the CI systems
First, you need to have the `cmake` option `RADIUM_COMPILE_TESTS` set to `ON` (the default).

To run the tests locally, you first need to compile them (`make buildtests`) and then call CTest (`make test`). You should get something like:
~~~text
    $ make test
    Running tests...
    Test project /home/me/build-Radium-Engine-Desktop-Release
          Start  1: test_core_algebra
     1/11 Test  #1: test_core_algebra ................   Passed    0.00 sec
          Start  2: test_core_animation
     2/11 Test  #2: test_core_animation ..............   Passed    0.01 sec
          Start  3: test_core_containers
     3/11 Test  #3: test_core_containers .............   Passed    0.00 sec
          Start  4: test_core_distance
     4/11 Test  #4: test_core_distance ...............   Passed    0.00 sec
          Start  5: test_core_geometry
     5/11 Test  #5: test_core_geometry ...............   Passed    0.00 sec
          Start  6: test_core_indexmap
     6/11 Test  #6: test_core_indexmap ...............   Passed    0.00 sec
          Start  7: test_core_mesh
     7/11 Test  #7: test_core_mesh ...................   Passed    0.00 sec
          Start  8: test_core_raycast
     8/11 Test  #8: test_core_raycast ................   Passed    0.01 sec
          Start  9: test_core_string
     9/11 Test  #9: test_core_string .................   Passed    0.00 sec
          Start 10: test_core_topomesh
    10/11 Test #10: test_core_topomesh ...............   Passed    0.01 sec
          Start 11: test_core_polyline
    11/11 Test #11: test_core_polyline ...............   Passed    0.00 sec

    100% tests passed, 0 tests failed out of 11

    Total Test time (real) =   0.06 sec
~~~

The standard output is not printed when using CTest.
To debug a test, you might need to run it directly. Tests binaries are located in `CMAKE_BUILD_PATH/tests/`.

Tests are automatically compiled and ran for Linux and MacOS CI, each time commits are pushed to Github. See snippet from TravisCI configuration file `Radium-Engine/.travis`:
~~~text
    script:
      - make -j 4
      - make buildtests && make test
~~~
