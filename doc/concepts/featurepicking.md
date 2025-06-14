\page develpicking  Feature Picking
[TOC]

# Usage

The RadiumEngine's Ra::Engine::Rendering::Renderer provides a picking method for Ra::Engine::Rendering::RenderObject selection.

The `FeaturePicking` feature enables to enhance the selection process up to selecting vertices, edges and triangles.
The result of a Ra::Engine::Rendering::Renderer::PickingQuery is a Ra::Engine::Rendering::Renderer::PickingResult containing the following data:

* the picking mode: either Ra::Engine::Rendering::Renderer::RO, Ra::Engine::Rendering::Renderer::VERTEX, Ra::Engine::Rendering::Renderer::EDGE, Ra::Engine::Rendering::Renderer::TRIANGLE for simple feature selection and either Ra::Engine::Rendering::Renderer::C_VERTEX, Ra::Engine::Rendering::Renderer::C_EDGE or Ra::Engine::Rendering::Renderer::C_TRIANGLE for multiple selection;
* the index of the RO the selection is for ;
* the list of selected vertices indices given w.r.t. the corresponding selected element ;
* the list of selected element indices, either points for point clouds or triangles for meshes ;
* the list of selected edge's opposite vertex indices given w.r.t. the corresponding selected triangle.

Right after a selection, the Ra::Engine::Rendering::Renderer::PickingResult are stored in the Ra::Gui::PickingManager and the Ra::Gui::SelectionManager emits Ra::Gui::SelectionManager::currentChanged signal so that Plugins are told a new selection has been made.

The selection of a mesh feature is activated by holding a specific key while selecting the object (see the `KeyMappingManager` configuration file for those). Multiple selection activation can be activated/de-activated by pressing the corresponding key, which would also make a circle shape, representing the selection area, appear on the screen.

# MeshFeatureTracking Plugin

The `MeshFeatureTracking` Plugin (**Radium Official Plugins** (<https://gitlab.com/Storm-IRIT/radium-official-plugins>)) is an example of how mesh features information can be displayed either through the `Gui` or as displayed objects.
The `SelectionManager`'s `currentChanged` signal is connected to the slot `onCurrentChanged`.

When this slot is triggered, the `MeshFeatureTracking` Plugin accesses the picking data through the `PickingManager` and updates both its information widget with the new picking data and the tracking sphere.
The `MeshFeatureTracking` plugin also keeps its information widget updated at the end of each frame.

The `MeshFeatureTracking` plugin only manage simple feature selection. For an example on how to deal with multiple selection, please see the `MeshPaint` Plugin.

# MeshPaint Plugin

The `MeshPaint` Plugin (**Radium Official Plugins** (<https://gitlab.com/Storm-IRIT/radium-official-plugins>)) is an example of how to use mesh feature multi-selection.
Here again, the `SelectionManager`'s `currentChanged` signal is connected to the slot `onCurrentChanged`.

When this slot is triggered, and if the painting is enabled, the `MeshPaint` Plugin directly changes the render color of the vertices corresponding to the current picking data.
