# MeshFeaturePicking Manual

## Usage

The RadiumEngine's `Renderer` provides a picking method enabling `RenderObject` selection.
The `MeshFeaturePicking` feature enables to enhance the selection process up to selecting mesh vertices, edges and triangles through a raycast querry on the selected `RenderObject`.
The result of a `MeshFeaturePicking` is a `FeatureData` containing the selection mode:
* `PickingMode::RO`
* `PickingMode::VERTEX`
* `PickingMode::EDGE`
* `PickingMode::TRIANGLE`

as well as the selected feature data, i.e. indices of its vertices.

The selection of a mesh feature (can can select only one feature at a time) is activated by holding a specific key while selecting the object (see the `KeyMappingManager` configuration file for those).

As of now, when a vertex or a triangle is selected, a dedicated UI layout pops up in the "Tracking" tab.
This layout contains the index of the vertex/triangle, which can be manually edited.

## MeshFeatureTracking Plugin

The `MeshFeatureTracking` Plugin is an example of how mesh features informations can be displayed either through the `Gui` or as displayed objets.
When selecting a mesh feature, the `SelectionManager` emits the `currentChanged` signal, which is connected to the `MeshFeatureTracking` Plugin slot `onCurrentChanged`.
Then, the `MeshFeatureTracking` Plugin updates its tracking widget and displayed object with respect to the selected mesh feature.
