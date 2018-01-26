# FeaturePicking Manual

## Usage

The RadiumEngine's `Renderer` provides a picking method for `RenderObject` selection.

The `FeaturePicking` feature enables to enhance the selection process up to selecting vertices, edges and triangles.
The result of a `PickingQuery` is a `PickingResult` containing the following data:
* the picking mode: either`PickingMode::RO`,`PickingMode::VERTEX`, `PickingMode::EDGE`,`PickingMode::TRIANGLE`for simple feature selection and either `C_VERTEX`, `C_EDGE` or `C_TRIANGLE` for multiple selection;
* the index of the RO the selection is for ;
* the list of selected vertices indices given w.r.t. the corresponding selected element ;
* the list of selected element indices, either points for point clouds or triangles for meshes ;
* the list of selected edge's opposite vertice indice given w.r.t. the corresponding selected triangle.

Right after a selection, the `PickingResults` are stored in the `PickingManager`and the `SelectionManager` emits the `currentChanged` signal so that Plugins are told a new selection has been made.

The selection of a mesh feature is activated by holding a specific key while selecting the object (see the `KeyMappingManager` configuration file for those). Multiple selection activation can be activated/de-ativated by pressing the corresponding key, which would also make a circle shape, representing the selection area, appear on the screen. 

## MeshFeatureTracking Plugin

The `MeshFeatureTracking` Plugin is an example of how mesh features informations can be displayed either through the `Gui` or as displayed objets.
The `SelectionManager`'s `currentChanged` signal is connected to the slot `onCurrentChanged`.

When this slot is triggered, the `MeshFeatureTracking` Plugin accesses the picking data through the `PickingManager` and updates both its information widget with the new picking data and the tracking sphere.
The `MeshFeatureTracking` plugin also keeps its information widget updated at the end of each frame.

The `MeshFeatureTracking` plugin only manage simple feature selection. For an example on how to deal with multiple selection, please see the `MeshPaint` Plugin (TODO).
