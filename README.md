# Radium-Engine libraries
Radium is a research 3D Engine for rendering, animation and processing.
It is developed and maintained by the [STORM research group](https://www.irit.fr/STORM/site/).

<p align="center">
  <b>
  [<a href="https://github.com/STORM-IRIT/Radium-Releases/releases">Click here to download Precompiled binaries</a>]
  </b>
 </p>

The Radium ecosystem is composed of
 - **Radium Libraries**: this project (see documentation here: https://storm-irit.github.io/Radium-Engine/). Contains the Radium libraries.
 - **Radium Apps** (https://github.com/STORM-IRIT/Radium-Apps): contains several applications, including MainApp, which is for now the default software with GUI. This repository also demonstrates command-line applications.
 - **Radium Releases** (https://github.com/STORM-IRIT/Radium-Releases): meta-repository generating precompiled binaries for Radium libraries and its applications. 
 - **Radium Plugins Example** (https://github.com/STORM-IRIT/Radium-PluginExample): contains plugins examples, demonstrating how to write, compile and use plugins with MainApp.
 - **Radium Official Plugins** (https://gitlab.com/Storm-IRIT/radium-official-plugins): general purpose plugins.

## Which repository should I use ?
Depending on you needs, you may want to:
 - Use Radium as a viewer and processing software, with graphical user interface. That's the most simple case: simply download the latest release, and run. See https://github.com/STORM-IRIT/Radium-Releases/releases.
 - Contribute to Radium, or develop your own application of plugin: **Radium Libraries** is the default and mandatory repository you need to fetch first. If you only want to _use_ Radium, compile and install it (see [documentation](https://storm-irit.github.io/Radium-Engine/)).
 Then,
   - If you want to contribute to the application or plugins developments, fetch and compile the **Radium Apps** and **Radium Official Plugins** respectively.
   - If you want to write your own application of plugin, checkout the [HelloRadium](https://github.com/STORM-IRIT/Radium-Apps/tree/fix-compilation/HelloRadium) and **Radium Plugins Example** projects.

## Badges

[![CI linux badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/nmellado/0e76e93f56eba8a7b28d6a0116873d85/raw/ubuntu-latest.json)](https://github.com/STORM-IRIT/Radium-Engine/actions?query=workflow%3A%22Compile+and+Test+Radium+libraries%22)
[![CI macos badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/nmellado/0e76e93f56eba8a7b28d6a0116873d85/raw/macos-latest.json)](https://github.com/STORM-IRIT/Radium-Engine/actions?query=workflow%3A%22Compile+and+Test+Radium+libraries%22)
[![CI windows  badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/nmellado/0e76e93f56eba8a7b28d6a0116873d85/raw/windows-latest.json)](https://github.com/STORM-IRIT/Radium-Engine/actions?query=workflow%3A%22Compile+and+Test+Radium+libraries%22)
[![source format badge](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/nmellado/0e76e93f56eba8a7b28d6a0116873d85/raw/format.json)](https://github.com/STORM-IRIT/Radium-Engine/actions?query=workflow%3A%22Compile+and+Test+Radium+libraries%22)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/faf8701c9fb142f7b6215871ec40c5fe)](https://app.codacy.com/app/STORM/Radium-Engine?utm_source=github.com&utm_medium=referral&utm_content=STORM-IRIT/Radium-Engine&utm_campaign=Badge_Grade_Dashboard)
[![codecov](https://codecov.io/gh/STORM-IRIT/Radium-Engine/branch/master/graph/badge.svg?token=MKfANkC3sd)](https://codecov.io/gh/STORM-IRIT/Radium-Engine)
