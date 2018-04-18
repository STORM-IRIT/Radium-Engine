# How to code in Radium

There are three main options to work with Radium:
1. Write a plugin: full access to the Radium data structures, perfect to implement a new fonctionality: mesh processing, rendering.
2. Write an application: give total control over the GUI, the camera settings, etc...
3. Contribute to Radium libraries: to improve/extend core components of Radium.

## Radium Plugin
Tutorial here: https://github.com/AGGA-IRIT/Radium-PluginExample

## Radium Application
Tutorial here: https://github.com/AGGA-IRIT/Radium-AppExample

## Radium libraries
Direct contributions to master are closed.
Please submit your pull request.

## Radium coding style
Please follow the scripts/clang-format coding style (tested `with clang-format 6.0`).
To use it, you have to copy or link `scripts/clang-format` to `.clang-format` (in Radium-Engine root dir).
We also provide a pre commit hook that checks the commited files are correctly formated.
To install both hooks and clang-format, simply run `./scripts/install-scripts-linux.sh` on linux, or adapt to your OS.
