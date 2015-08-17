# Scene description file

Radium-Engine uses JSON files to describe a whole scene (entities and components 
for each entity that are required to be loaded), camera, lights, etc.

Camera is provided by a JSon object (under the key `"camera"`), it has the following properties :

*not implemeted yet*

Lights are provided by a JSon array (under the key `"lights"`), a light has the following properties :

 *not implemented yet*

Entities are provided by a JSon array (under the key `"entities"`), an entity has the following properties :
 * Name (string) *required*
 * Position (array of 3 float) : XYZ Vector that defaults to [0, 0, 0] *optionnal*
 * Orientation (array of 4 float) : WXYZ Quaternion that defaults to [1, 0, 0, 0] *optionnal*
 * Scale (array of 3 float) : XYZ Vector that defaults to [1, 1, 1] *optionnal*
 * And array of components, including the name of the system, see example below
 
For the sake of the example, let's consider that we have two systems, and the data we want 
to load associated : 
 * `FancyMeshSystem`
   * `mesh` (string) 
   * `transparent` (bool)
   
 * `RigidBodySystem`
   * `kinematic` (bool)
   * `mass` (float) 
 
For our example, the JSON file might look like 

```
{
    "entity":
    [{
      "name": "Huge transparent falling box",
      "orientation": [0.854, -0.354, 0.146, 0.354],
      "scale": [42, 42, 42],
        
      "components":
      [{
        "system": "FancyMeshSystem",
        "mesh": "box.obj",
        "transparent": true
      },
        
      {
        "system": "RigidBodySystem",
        "mass": 1.0
      }]
    },
    
    {
      "name": "Tiny opaque kinematic sphere",
      "position": [42, 42, 42],
      "scale": [0.1, 0.1, 0.1],
        
      "components":
      [{
        "system": "FancyMeshSystem",
        "mesh": "sphere.obj",
        "transparent": false
      },
        
      {
        "system": "RigidBodySystem",
        "kinematic": true
      }]
    }]
}
```
