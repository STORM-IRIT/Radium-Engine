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
    [ "Radium Basics", "md_basics.html", null ],
    [ "Radium Concepts", "md_concepts.html", null ],
    [ "Developer manual", "md_developer.html", "md_developer" ],
    [ "Licenses", "md_LICENSES.html", [
      [ "Radium contributors", "md_LICENSES.html#autotoc_md3", null ],
      [ "Third-party libraries", "md_LICENSES.html#autotoc_md4", [
        [ "Qt", "md_LICENSES.html#autotoc_md5", null ],
        [ "Eigen", "md_LICENSES.html#autotoc_md6", null ],
        [ "Assimp", "md_LICENSES.html#autotoc_md7", null ],
        [ "STB Image", "md_LICENSES.html#autotoc_md8", null ],
        [ "Additional material", "md_LICENSES.html#autotoc_md9", null ]
      ] ]
    ] ],
    [ "Radium Compilation instructions", "md_basics_compilation.html", [
      [ "Supported compiler and platforms", "md_basics_compilation.html#autotoc_md10", null ],
      [ "Build instructions", "md_basics_compilation.html#autotoc_md11", [
        [ "Folder structure", "md_basics_compilation.html#autotoc_md12", null ],
        [ "Command line instructions for building (on windows, mac and linux)", "md_basics_compilation.html#autotoc_md13", null ],
        [ "Integration with Visual Studio (Microsoft Windows)", "md_basics_compilation.html#autotoc_md14", null ]
      ] ]
    ] ],
    [ "Radium Dependencies Management", "md_basics_dependencies.html", [
      [ "Dependencies management systems", "md_basics_dependencies.html#autotoc_md15", null ],
      [ "Building and installing Radium dependencies once for all", "md_basics_dependencies.html#builddep", [
        [ "Configuration and compilation of the dependencies", "md_basics_dependencies.html#autotoc_md16", null ],
        [ "Configuration of Radium", "md_basics_dependencies.html#autotoc_md17", null ]
      ] ],
      [ "User provided external dependencies", "md_basics_dependencies.html#autotoc_md18", null ]
    ] ],
    [ "Use Radium Libraries in your own project", "md_basics_radium_as_submodule.html", null ],
    [ "Troubleshooting", "md_basics_troubleshooting.html", [
      [ "Qt cmake errors", "md_basics_troubleshooting.html#autotoc_md21", null ],
      [ "Crash when starting main application on windows", "md_basics_troubleshooting.html#autotoc_md22", null ]
    ] ],
    [ "[deprecated] Component Messenger", "md_concepts_componentMessenger.html", [
      [ "Read", "md_concepts_componentMessenger.html#autotoc_md23", null ],
      [ "Write", "md_concepts_componentMessenger.html#autotoc_md24", null ],
      [ "Read'n'Write", "md_concepts_componentMessenger.html#autotoc_md25", null ],
      [ "The <tt>setupIO()</tt> function", "md_concepts_componentMessenger.html#autotoc_md26", null ]
    ] ],
    [ "[deprecated] Event System", "md_concepts_eventSystem.html", [
      [ "Notes for event handling the radium engine", "md_concepts_eventSystem.html#autotoc_md27", [
        [ "Events identification", "md_concepts_eventSystem.html#autotoc_md28", null ],
        [ "Events registering", "md_concepts_eventSystem.html#autotoc_md29", null ],
        [ "Events queue", "md_concepts_eventSystem.html#autotoc_md30", null ]
      ] ]
    ] ],
    [ "Forward Render", "md_concepts_forwardrenderer.html", [
      [ "1. Depth, ambient color and \"deferred info\" pass", "md_concepts_forwardrenderer.html#autotoc_md31", null ],
      [ "2. Lighting pass", "md_concepts_forwardrenderer.html#autotoc_md32", null ],
      [ "3. Ordered independent transparency", "md_concepts_forwardrenderer.html#autotoc_md33", null ],
      [ "4. Post-process the whole <em>render pass</em>", "md_concepts_forwardrenderer.html#autotoc_md34", null ],
      [ "5. Debug and UI render objects", "md_concepts_forwardrenderer.html#autotoc_md35", null ],
      [ "6. Write final texture to framebuffer / backbuffer", "md_concepts_forwardrenderer.html#autotoc_md36", null ]
    ] ],
    [ "Plugin System", "md_concepts_pluginSystem.html", [
      [ "Basic", "md_concepts_pluginSystem.html#autotoc_md37", null ],
      [ "Plugin", "md_concepts_pluginSystem.html#autotoc_md38", null ],
      [ "System", "md_concepts_pluginSystem.html#autotoc_md39", null ],
      [ "Component", "md_concepts_pluginSystem.html#autotoc_md40", null ],
      [ "Task", "md_concepts_pluginSystem.html#autotoc_md41", null ]
    ] ],
    [ "Animation pipeline", "md_developer_animation.html", [
      [ "Animation in Radium", "md_developer_animation.html#autotoc_md42", [
        [ "Using KeyFramedValue to animate data", "md_developer_animation.html#autotoc_md43", null ],
        [ "Character Animation", "md_developer_animation.html#autotoc_md44", null ]
      ] ]
    ] ],
    [ "Camera manipulation in Radium", "md_developer_cameraManipulator.html", [
      [ "Implementing a CameraManipulator", "md_developer_cameraManipulator.html#autotoc_md45", null ],
      [ "Extending/Specializing an existing CameraManipulator", "md_developer_cameraManipulator.html#autotoc_md46", null ],
      [ "Using a CameraManipulator", "md_developer_cameraManipulator.html#autotoc_md47", null ]
    ] ],
    [ "How to use Radium : cmake configuration and utilities", "md_developer_cmakeutilities.html", [
      [ "Radium cmake utilities", "md_developer_cmakeutilities.html#autotoc_md48", [
        [ "Using Radium in your application", "md_developer_cmakeutilities.html#autotoc_md49", null ],
        [ "Client application configuration", "md_developer_cmakeutilities.html#autotoc_md50", [
          [ "configure_radium_app", "md_developer_cmakeutilities.html#configure_radium_app", null ]
        ] ],
        [ "Extending Radium through libraries", "md_developer_cmakeutilities.html#autotoc_md51", [
          [ "configure_radium_library", "md_developer_cmakeutilities.html#configure_radium_library", null ],
          [ "install_target_resources", "md_developer_cmakeutilities.html#install_target_resources", null ],
          [ "configure_radium_package", "md_developer_cmakeutilities.html#configurePackage", null ],
          [ "radium_exported_resources", "md_developer_cmakeutilities.html#radium_exported_resources", null ]
        ] ],
        [ "Extending Radium through plugins", "md_developer_cmakeutilities.html#autotoc_md52", [
          [ "configure_radium_plugin", "md_developer_cmakeutilities.html#configure_radium_plugin", null ]
        ] ],
        [ "Adding a new component to the Radium package", "md_developer_cmakeutilities.html#autotoc_md53", null ]
      ] ],
      [ "How to write your CMakeLists.txt", "md_developer_cmakeutilities.html#autotoc_md54", [
        [ "General cmake preamble", "md_developer_cmakeutilities.html#autotoc_md55", null ],
        [ "Configuring client application", "md_developer_cmakeutilities.html#autotoc_md56", null ],
        [ "Configuring client and extension libraries", "md_developer_cmakeutilities.html#configureLibrary", null ],
        [ "Configuring an application plugin", "md_developer_cmakeutilities.html#autotoc_md57", null ]
      ] ]
    ] ],
    [ "Coding conventions for Radium", "md_developer_conventions.html", [
      [ "Code style", "md_developer_conventions.html#autotoc_md58", null ],
      [ "Headers", "md_developer_conventions.html#autotoc_md59", null ],
      [ "Functions", "md_developer_conventions.html#autotoc_md60", null ],
      [ "Scope and names", "md_developer_conventions.html#autotoc_md61", null ],
      [ "Variables", "md_developer_conventions.html#autotoc_md62", null ],
      [ "Scalar types", "md_developer_conventions.html#autotoc_md63", null ],
      [ "Class design", "md_developer_conventions.html#autotoc_md64", null ],
      [ "Non-negociable", "md_developer_conventions.html#autotoc_md65", null ]
    ] ],
    [ "How to contribute to Radium Libraries", "md_developer_develWithRadium.html", [
      [ "Radium libraries", "md_developer_develWithRadium.html#autotoc_md66", null ],
      [ "Pre commit hook", "md_developer_develWithRadium.html#autotoc_md67", null ],
      [ "Radium test suite", "md_developer_develWithRadium.html#autotoc_md68", [
        [ "Preliminaries", "md_developer_develWithRadium.html#autotoc_md69", null ],
        [ "How to write tests", "md_developer_develWithRadium.html#autotoc_md70", null ]
      ] ],
      [ "Code coverage", "md_developer_develWithRadium.html#autotoc_md71", null ]
    ] ],
    [ "Feature Picking", "md_developer_featurepicking.html", [
      [ "Usage", "md_developer_featurepicking.html#autotoc_md72", null ],
      [ "MeshFeatureTracking Plugin", "md_developer_featurepicking.html#autotoc_md73", null ],
      [ "MeshPaint Plugin", "md_developer_featurepicking.html#autotoc_md74", null ]
    ] ],
    [ "KeyMapping", "md_developer_keymapping.html", [
      [ "Usage", "md_developer_keymapping.html#autotoc_md75", null ],
      [ "Implementation note", "md_developer_keymapping.html#autotoc_md76", null ],
      [ "Key mapping and inheritence", "md_developer_keymapping.html#autotoc_md77", null ],
      [ "Limits", "md_developer_keymapping.html#autotoc_md78", null ]
    ] ],
    [ "Lights", "md_developer_lightmanagement.html", [
      [ "Light Manager", "md_developer_lightmanagement.html#autotoc_md79", null ],
      [ "Loading lights", "md_developer_lightmanagement.html#autotoc_md80", null ],
      [ "Using light", "md_developer_lightmanagement.html#autotoc_md81", null ]
    ] ],
    [ "Material management", "md_developer_material.html", [
      [ "Content of the Radium Material Library", "md_developer_material.html#autotoc_md82", null ],
      [ "Simple usage of materials", "md_developer_material.html#simple-mtl-usage", null ],
      [ "Extending the Radium Material Library", "md_developer_material.html#extend-mtl-lib", [
        [ "C++ interface", "md_developer_material.html#cpp-mtl-lib", [
          [ "Making a material editable", "md_developer_material.html#editable-interface", null ]
        ] ],
        [ "GLSL interface", "md_developer_material.html#glsl-mtl-lib", null ],
        [ "Appearance computation needs", "md_developer_material.html#autotoc_md83", [
          [ "Vertex attrib interface", "md_developer_material.html#vrtx-attr-interface", null ],
          [ "Microgeometry interface", "md_developer_material.html#microgeometry-interface", null ],
          [ "BSDF interface", "md_developer_material.html#bsdf-interface", null ],
          [ "Emissivity interface", "md_developer_material.html#emissivity-interface", null ]
        ] ],
        [ "Material registration into the Engine", "md_developer_material.html#registration-mtl-lib", null ],
        [ "Rendering without using Materials", "md_developer_material.html#non-bsdf-rendering", null ]
      ] ],
      [ "\\todo TO UPDATE", "md_developer_material.html#autotoc_md84", [
        [ "Manipulating ShaderConfigurations", "md_developer_material.html#autotoc_md85", null ]
      ] ]
    ] ],
    [ "3D objects (a.k.a. Meshes)", "md_developer_mesh.html", [
      [ "Geometry types", "md_developer_mesh.html#autotoc_md86", null ],
      [ "Colaboration between Core and Engine", "md_developer_mesh.html#autotoc_md87", null ],
      [ "Data consistency", "md_developer_mesh.html#autotoc_md88", null ],
      [ "Mesh creation", "md_developer_mesh.html#autotoc_md89", [
        [ "Wedges", "md_developer_mesh.html#wedges", null ]
      ] ]
    ] ],
    [ "[deprecated] Radium Engine programmer manual", "md_developer_oldmanual.html", [
      [ "Source code organization", "md_developer_oldmanual.html#autotoc_md90", [
        [ "Import and export", "md_developer_oldmanual.html#autotoc_md91", null ]
      ] ],
      [ "Core Libs", "md_developer_oldmanual.html#autotoc_md92", null ],
      [ "Engine Object Model", "md_developer_oldmanual.html#autotoc_md93", [
        [ "Entities", "md_developer_oldmanual.html#autotoc_md94", null ],
        [ "Systems and Components", "md_developer_oldmanual.html#autotoc_md95", null ],
        [ "Frame and Tasks", "md_developer_oldmanual.html#autotoc_md96", null ],
        [ "Rendering", "md_developer_oldmanual.html#autotoc_md97", null ],
        [ "ItemEntry : Handles to engine objects", "md_developer_oldmanual.html#autotoc_md98", null ],
        [ "Object manipulation and editable transforms", "md_developer_oldmanual.html#autotoc_md99", null ],
        [ "The system Entity and Debug Display", "md_developer_oldmanual.html#autotoc_md100", null ]
      ] ],
      [ "The plugin framework", "md_developer_oldmanual.html#autotoc_md101", [
        [ "Requirements", "md_developer_oldmanual.html#autotoc_md102", null ],
        [ "Default plugins", "md_developer_oldmanual.html#autotoc_md103", null ]
      ] ]
    ] ],
    [ "How to write your own plugin", "md_developer_plugin.html", [
      [ "Radium Plugin", "md_developer_plugin.html#autotoc_md104", null ],
      [ "Setting a CMakeLists.txt for a Radium plugin", "md_developer_plugin.html#autotoc_md105", null ],
      [ "Configuring the plugin", "md_developer_plugin.html#autotoc_md106", null ],
      [ "Using the Plugin", "md_developer_plugin.html#autotoc_md107", null ]
    ] ],
    [ "Rendering pipeline", "md_developer_rendering.html", [
      [ "Rendering in Radium", "md_developer_rendering.html#autotoc_md108", [
        [ "Main abstract render method", "md_developer_rendering.html#autotoc_md109", null ],
        [ "Defining a concrete renderer", "md_developer_rendering.html#autotoc_md110", null ],
        [ "Minimal renderer howto", "md_developer_rendering.html#autotoc_md111", null ]
      ] ],
      [ "Radium Engine default rendering information", "md_developer_rendering.html#autotoc_md112", null ],
      [ "Potential extensions for the renderer", "md_developer_rendering.html#autotoc_md113", null ]
    ] ],
    [ "Timeline and Keyframes", "md_developer_timeline.html", [
      [ "Timeline And Keyframes", "md_developer_timeline.html#autotoc_md114", [
        [ "The Timeline UI", "md_developer_timeline.html#autotoc_md115", null ],
        [ "Display animated data using the Timeline", "md_developer_timeline.html#autotoc_md116", null ]
      ] ]
    ] ],
    [ "Todo List", "todo.html", null ],
    [ "Deprecated List", "deprecated.html", null ],
    [ "Modules", "modules.html", "modules" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
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
        [ "Related Functions", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"AboutDialog_8cpp_source.html",
"PickingManager_8hpp_source.html",
"classRa_1_1Core_1_1Animation_1_1HandleArray.html#a8d51faa3b4fa49a0056d28c830860c4da7a03bac2692f8d17e2cda1b0721ffd0e",
"classRa_1_1Core_1_1Asset_1_1Camera.html#a74ae04c9a3018609a6468eaa59928ee5",
"classRa_1_1Core_1_1Asset_1_1MaterialData.html",
"classRa_1_1Core_1_1Geometry_1_1MultiIndexedGeometry.html#a6192bea8770876bd8d0f22e56ac7799e",
"classRa_1_1Core_1_1Geometry_1_1deprecated_1_1TopologicalMesh.html#a720ac0a99914c6467ce104d3432d32f9",
"classRa_1_1Core_1_1Utils_1_1AttribManager.html#a191ecf090eaf0b6d52ed390938e1e0c5",
"classRa_1_1Engine_1_1Data_1_1AttribArrayDisplayable.html#a9832e391e3bf85e7900ea3b22145da40",
"classRa_1_1Engine_1_1Data_1_1PointCloud.html",
"classRa_1_1Engine_1_1Data_1_1Texture.html#ac7be6d3a2a0e9ef55c298c6e3b914f96",
"classRa_1_1Engine_1_1Rendering_1_1RenderTechnique.html#a6b2bfb1ead4afbf234689c0ae411c1ec",
"classRa_1_1Engine_1_1Scene_1_1ComponentMessenger.html#abaebd35c0c84995568869d8fe07f915c",
"classRa_1_1Engine_1_1Scene_1_1SkeletonComponent.html#aa8a61a85c5424e6833596fb431c30cb1",
"classRa_1_1Gui_1_1CameraManipulator.html#aa9539d63bb185b615ddc996c6cca5dfe",
"classRa_1_1Gui_1_1SimpleWindow.html#ad4bc4c6844a2a5380d2b9d8339d99641",
"classRa_1_1Gui_1_1Viewer.html#a02c5ab9153a9701e8f14589a4589f284",
"classRa_1_1IO_1_1AssimpAnimationDataLoader.html#a3418a4388826275e05fb600e4769d9d7",
"md_LICENSES.html#autotoc_md7",
"structRa_1_1Core_1_1Asset_1_1HandleAnimation.html#ad8b2830eb3588f4c54a890e8ef389b60"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';