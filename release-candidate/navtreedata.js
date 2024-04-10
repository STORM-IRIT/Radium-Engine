/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Radium Engine", "index.html", [
    [ "Badges", "index.html#autotoc_md0", null ],
    [ "Overview", "index.html#autotoc_md1", null ],
    [ "Radium Basics", "basicsmanual.html", [
      [ "Microsoft Visual Studio Compilation Instructions", "basicsCompileVs.html", [
        [ "Supported system and compilers", "basicsmanual.html#autotoc_md2", null ],
        [ "System wide dependencies", "basicsmanual.html#autotoc_md3", null ],
        [ "External dependencies (included with radium)", "basicsmanual.html#autotoc_md4", null ],
        [ "TL;DR; command line version", "basicsmanual.html#autotoc_md5", null ],
        [ "Detailed instructions", "basicsmanual.html#autotoc_md6", null ],
        [ "Build external dependencies", "basicsCompileVs.html#autotoc_md27", null ],
        [ "Build Radium", "basicsCompileVs.html#autotoc_md28", [
          [ "Compilation", "basicsCompileVs.html#autotoc_md29", null ],
          [ "Execution of a demo app", "basicsCompileVs.html#autotoc_md30", null ],
          [ "installation", "basicsCompileVs.html#autotoc_md31", null ]
        ] ]
      ] ],
      [ "Commandline Compilation Instructions (Linux, macos...)", "basicsCompileCommand.html", [
        [ "Building and installing Radium dependencies once for all", "basicsCompileCommand.html#builddep", [
          [ "Configuration and compilation of the dependencies", "basicsCompileCommand.html#autotoc_md15", null ],
          [ "Command line instructions (linux, macos, windows)", "basicsCompileCommand.html#autotoc_md16", null ]
        ] ]
      ] ],
      [ "Use Radium Libraries in your own project", "basicsRadiumSubmodule.html", null ],
      [ "Radium Compilation Details", "basicsCompileRadiumDetails.html", [
        [ "Dependencies management systems", "basicsCompileRadiumDetails.html#autotoc_md17", [
          [ "Configuration of Radium", "basicsCompileRadiumDetails.html#autotoc_md18", null ],
          [ "User provided external dependencies", "basicsCompileRadiumDetails.html#autotoc_md19", null ]
        ] ],
        [ "Radium Build Details", "basicsCompileRadiumDetails.html#autotoc_md20", [
          [ "Folder structure", "basicsCompileRadiumDetails.html#autotoc_md21", null ],
          [ "Configure build options", "basicsCompileRadiumDetails.html#autotoc_md22", null ]
        ] ]
      ] ],
      [ "Troubleshooting", "basicsTroubleshooting.html", [
        [ "Qt cmake errors", "basicsTroubleshooting.html#autotoc_md25", null ],
        [ "Crash when starting main application on windows", "basicsTroubleshooting.html#autotoc_md26", null ]
      ] ]
    ] ],
    [ "Radium Concepts", "conceptsmanual.html", [
      [ "_deprecated_ Component Messenger", "componentMessenger.html", [
        [ "Read", "componentMessenger.html#autotoc_md32", null ],
        [ "Write", "componentMessenger.html#autotoc_md33", null ],
        [ "Read'n'Write", "componentMessenger.html#autotoc_md34", null ],
        [ "The <tt>setupIO()</tt> function", "componentMessenger.html#autotoc_md35", null ]
      ] ],
      [ "_deprecated_ Event System", "eventSystem.html", [
        [ "Notes for event handling the radium engine", "eventSystem.html#autotoc_md36", [
          [ "Events identification", "eventSystem.html#autotoc_md37", null ],
          [ "Events registering", "eventSystem.html#autotoc_md38", null ],
          [ "Events queue", "eventSystem.html#autotoc_md39", null ]
        ] ]
      ] ],
      [ "Plugin System", "pluginSystem.html", [
        [ "Basic", "pluginSystem.html#autotoc_md46", null ],
        [ "Plugin", "pluginSystem.html#autotoc_md47", null ],
        [ "System", "pluginSystem.html#autotoc_md48", null ],
        [ "Component", "pluginSystem.html#autotoc_md49", null ],
        [ "Task", "pluginSystem.html#autotoc_md50", null ]
      ] ],
      [ "Forward Render", "forwardRenderer.html", [
        [ "1. Depth, ambient color and \"deferred info\" pass", "forwardRenderer.html#autotoc_md40", null ],
        [ "2. Lighting pass", "forwardRenderer.html#autotoc_md41", null ],
        [ "3. Ordered independent transparency", "forwardRenderer.html#autotoc_md42", null ],
        [ "4. Post-process the whole <em>render pass</em>", "forwardRenderer.html#autotoc_md43", null ],
        [ "5. Debug and UI render objects", "forwardRenderer.html#autotoc_md44", null ],
        [ "6. Write final texture to framebuffer / backbuffer", "forwardRenderer.html#autotoc_md45", null ]
      ] ]
    ] ],
    [ "Developer manual", "develmanual.html", [
      [ "How to use Radium : cmake configuration and utilities", "cmakeutilities.html", [
        [ "Radium cmake utilities", "cmakeutilities.html#autotoc_md57", [
          [ "Using Radium in your application", "cmakeutilities.html#autotoc_md58", null ],
          [ "Client application configuration", "cmakeutilities.html#autotoc_md59", [
            [ "configure_radium_app", "cmakeutilities.html#configure_radium_app", null ]
          ] ],
          [ "Extending Radium through libraries", "cmakeutilities.html#autotoc_md60", [
            [ "configure_radium_library", "cmakeutilities.html#configure_radium_library", null ],
            [ "install_target_resources", "cmakeutilities.html#install_target_resources", null ],
            [ "configure_radium_package", "cmakeutilities.html#configure_radium_package", null ],
            [ "radium_exported_resources", "cmakeutilities.html#radium_exported_resources", null ]
          ] ],
          [ "Extending Radium through plugins", "cmakeutilities.html#autotoc_md61", [
            [ "configure_radium_plugin", "cmakeutilities.html#configure_radium_plugin", null ]
          ] ],
          [ "Adding a new component to the Radium package", "cmakeutilities.html#autotoc_md62", null ]
        ] ],
        [ "How to write your CMakeLists.txt", "cmakeutilities.html#autotoc_md63", [
          [ "General cmake preamble", "cmakeutilities.html#autotoc_md64", null ],
          [ "Configuring client application", "cmakeutilities.html#autotoc_md65", null ],
          [ "Configuring client and extension libraries", "cmakeutilities.html#configureLibrary", null ],
          [ "Configuring an application plugin", "cmakeutilities.html#autotoc_md66", null ]
        ] ]
      ] ],
      [ "How to write your own plugin", "develplugin.html", [
        [ "Radium Plugin", "develplugin.html#autotoc_md114", null ],
        [ "Setting a CMakeLists.txt for a Radium plugin", "develplugin.html#autotoc_md115", null ],
        [ "Configuring the plugin", "develplugin.html#autotoc_md116", null ],
        [ "Using the Plugin", "develplugin.html#autotoc_md117", null ]
      ] ],
      [ "How to write your own application", "apps.html", [
        [ "Radium Application", "apps.html#autotoc_md7", null ]
      ] ],
      [ "How to contribute to Radium Libraries", "develWithRadium.html", [
        [ "Radium libraries", "develWithRadium.html#autotoc_md75", null ],
        [ "Pre commit hook", "develWithRadium.html#pre_commit", null ],
        [ "Radium test suite", "develWithRadium.html#autotoc_md76", [
          [ "Preliminaries", "develWithRadium.html#autotoc_md77", null ],
          [ "How to write tests", "develWithRadium.html#autotoc_md78", null ]
        ] ],
        [ "Code coverage", "develWithRadium.html#autotoc_md79", null ]
      ] ],
      [ "Coding conventions for Radium", "develCodingConvention.html", [
        [ "Code style", "develCodingConvention.html#autotoc_md67", null ],
        [ "Headers", "develCodingConvention.html#autotoc_md68", null ],
        [ "Functions", "develCodingConvention.html#autotoc_md69", null ],
        [ "Scope and names", "develCodingConvention.html#autotoc_md70", null ],
        [ "Variables", "develCodingConvention.html#autotoc_md71", null ],
        [ "Scalar types", "develCodingConvention.html#autotoc_md72", null ],
        [ "Class design", "develCodingConvention.html#autotoc_md73", null ],
        [ "Non-negociable", "develCodingConvention.html#autotoc_md74", null ]
      ] ],
      [ "API", "api.html", [
        [ "Core", "core.html", [
          [ "3D objects (a.k.a. Meshes)", "develmeshes.html", [
            [ "Geometry types", "develmeshes.html#autotoc_md96", null ],
            [ "Colaboration between Core and Engine", "develmeshes.html#autotoc_md97", null ],
            [ "Data consistency", "develmeshes.html#autotoc_md98", null ],
            [ "Mesh creation", "develmeshes.html#autotoc_md99", [
              [ "Wedges", "develmeshes.html#wedges", null ]
            ] ]
          ] ],
          [ "Animation pipeline", "develanimation.html", [
            [ "Animation in Radium", "develanimation.html#autotoc_md51", [
              [ "Using KeyFramedValue to animate data", "develanimation.html#autotoc_md52", null ],
              [ "Character Animation", "develanimation.html#autotoc_md53", null ]
            ] ]
          ] ]
        ] ],
        [ "Engine", "engine.html", [
          [ "Rendering pipeline", "develrendering.html", [
            [ "Rendering in Radium", "develrendering.html#autotoc_md118", [
              [ "Main abstract render method", "develrendering.html#autotoc_md119", null ],
              [ "Defining a concrete renderer", "develrendering.html#autotoc_md120", null ],
              [ "Minimal renderer howto", "develrendering.html#autotoc_md121", null ]
            ] ],
            [ "Radium Engine default rendering information", "develrendering.html#autotoc_md122", null ],
            [ "Potential extensions for the renderer", "develrendering.html#autotoc_md123", null ]
          ] ],
          [ "Textures management", "develTextures.html", null ],
          [ "Material management", "develmaterials.html", [
            [ "Content of the Radium Material Library", "develmaterials.html#autotoc_md92", null ],
            [ "Simple usage of materials", "develmaterials.html#simple-mtl-usage", null ],
            [ "Extending the Radium Material Library", "develmaterials.html#extend-mtl-lib", [
              [ "C++ interface", "develmaterials.html#cpp-mtl-lib", null ],
              [ "Textures", "develmaterials.html#material-textures", [
                [ "Making a material editable", "develmaterials.html#editable-interface", null ]
              ] ],
              [ "GLSL interface", "develmaterials.html#glsl-mtl-lib", null ],
              [ "Appearance computation needs", "develmaterials.html#autotoc_md93", [
                [ "Vertex attrib interface", "develmaterials.html#vrtx-attr-interface", null ],
                [ "Microgeometry interface", "develmaterials.html#microgeometry-interface", null ],
                [ "BSDF interface", "develmaterials.html#bsdf-interface", null ],
                [ "Emissivity interface", "develmaterials.html#emissivity-interface", null ]
              ] ],
              [ "Material registration into the Engine", "develmaterials.html#registration-mtl-lib", [
                [ "Registering a RenderTechnique", "develmaterials.html#render-technique", null ]
              ] ],
              [ "Rendering without using Materials", "develmaterials.html#non-bsdf-rendering", null ]
            ] ],
            [ "TO UPDATE", "develmaterials.html#autotoc_md94", [
              [ "Manipulating ShaderConfigurations", "develmaterials.html#autotoc_md95", null ]
            ] ]
          ] ],
          [ "Lights", "develLights.html", [
            [ "Light Manager", "develLights.html#autotoc_md89", null ],
            [ "Loading lights", "develLights.html#autotoc_md90", null ],
            [ "Using light", "develLights.html#autotoc_md91", null ]
          ] ],
          [ "Feature Picking", "develpicking.html", [
            [ "Usage", "develpicking.html#autotoc_md80", null ],
            [ "MeshFeatureTracking Plugin", "develpicking.html#autotoc_md81", null ],
            [ "MeshPaint Plugin", "develpicking.html#autotoc_md82", null ]
          ] ],
          [ "Timeline and Keyframes", "develtimeline.html", [
            [ "Timeline And Keyframes", "develtimeline.html#autotoc_md124", [
              [ "The Timeline UI", "develtimeline.html#autotoc_md125", null ],
              [ "Display animated data using the Timeline", "develtimeline.html#autotoc_md126", null ]
            ] ]
          ] ]
        ] ],
        [ "Gui", "gui.html", [
          [ "KeyMapping", "develkeymapping.html", null ],
          [ "Camera manipulation in Radium", "develCameraManipulator.html", [
            [ "Implementing a CameraManipulator", "develCameraManipulator.html#autotoc_md54", null ],
            [ "Extending/Specializing an existing CameraManipulator", "develCameraManipulator.html#autotoc_md55", null ],
            [ "Using a CameraManipulator", "develCameraManipulator.html#autotoc_md56", null ]
          ] ]
        ] ],
        [ "[TODO] IO", "io.html", null ]
      ] ],
      [ "_deprecated_ Radium Engine programmer manual", "develoldmanual.html", [
        [ "Source code organization", "develoldmanual.html#autotoc_md100", [
          [ "Import and export", "develoldmanual.html#autotoc_md101", null ]
        ] ],
        [ "Core Libs", "develoldmanual.html#autotoc_md102", null ],
        [ "Engine Object Model", "develoldmanual.html#autotoc_md103", [
          [ "Entities", "develoldmanual.html#autotoc_md104", null ],
          [ "Systems and Components", "develoldmanual.html#autotoc_md105", null ],
          [ "Frame and Tasks", "develoldmanual.html#autotoc_md106", null ],
          [ "Rendering", "develoldmanual.html#autotoc_md107", null ],
          [ "ItemEntry : Handles to engine objects", "develoldmanual.html#autotoc_md108", null ],
          [ "Object manipulation and editable transforms", "develoldmanual.html#autotoc_md109", null ],
          [ "The system Entity and Debug Display", "develoldmanual.html#autotoc_md110", null ]
        ] ],
        [ "The plugin framework", "develoldmanual.html#autotoc_md111", [
          [ "Requirements", "develoldmanual.html#autotoc_md112", null ],
          [ "Default plugins", "develoldmanual.html#autotoc_md113", null ]
        ] ]
      ] ]
    ] ],
    [ "Licenses", "licenses.html", [
      [ "Radium contributors", "licenses.html#autotoc_md8", null ],
      [ "Third-party libraries", "licenses.html#autotoc_md9", [
        [ "Qt", "licenses.html#autotoc_md10", null ],
        [ "Eigen", "licenses.html#autotoc_md11", null ],
        [ "Assimp", "licenses.html#autotoc_md12", null ],
        [ "STB Image", "licenses.html#autotoc_md13", null ],
        [ "Additional material", "licenses.html#autotoc_md14", null ]
      ] ]
    ] ],
    [ "Todo List", "todo.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ],
        [ "Enumerator", "namespacemembers_eval.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Enumerator", "functions_eval.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"AboutDialog_8cpp_source.html",
"Sequence_8cpp_source.html",
"classRa_1_1Core_1_1Animation_1_1Skeleton.html#a5b8b7122faf6e4c0313bf3946ce0a801",
"classRa_1_1Core_1_1Geometry_1_1AbstractVolume.html#a464097665750f5d25a9f5aa40f1ff2e2",
"classRa_1_1Core_1_1Grid.html#a8e692ae5c663d4ac79ad652b0c79caa2",
"classRa_1_1Core_1_1Utils_1_1IndexMap.html#ae623a78346f941adcc39554efe3b5988",
"classRa_1_1Engine_1_1Data_1_1EnvironmentTexture.html#a75c156ec8a5ff5ecd64bcee38cf1aaea",
"classRa_1_1Engine_1_1Data_1_1Texture.html#a04f41cf6b664f77b4077b78b2f566422",
"classRa_1_1Engine_1_1Rendering_1_1ForwardRenderer.html#aee5b499807dcaf78eee01db6dc5b5467",
"classRa_1_1Engine_1_1Scene_1_1CameraManager.html#ad4516473cebc8b540d2da18319985125",
"classRa_1_1Engine_1_1Scene_1_1SkeletonBasedAnimationSystem.html#ae5b707dccf07a57b033d2a47640e57b3",
"classRa_1_1Gui_1_1CameraManipulator.html#ac74d63a529a9dd1b6706ddefaa5070ea",
"classRa_1_1Gui_1_1TreeModel.html#abf4475fa48a1bfc17682cf91d5354c5e",
"cpp/algorithm/min.html",
"cpp/numeric/math/abs.html",
"develTextures.html",
"namespaceRa_1_1Core_1_1Math.html#a5f25a31a29b0688e349dc35a353b9766",
"structRa_1_1Engine_1_1Scene_1_1ComponentMessenger_1_1CallbackTypes.html#a69156f044994d1cbcf666dbe97360acf"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';