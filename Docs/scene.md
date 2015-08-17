# Scene description file

Radium-Engine uses JSON files to describe a whole scene (entities and components 
for each entity that are required to be loaded).

For the sake of the example, let's consider that we have two systems, and the data we want 
to load associated : 
 * `FancyMeshSystem`
   * `mesh` (string) 
   * `transparent` (bool)
   
 * `RigidBodySystem`
   * `kinematic` (bool)
   * `mass` (float)
   
Entities also can / have to be provided some description :
 * Name (string) *required*
 * Position (array of 3 float) : XYZ Vector that defaults to [0, 0, 0] *optionnal*
 * Orientation (array of 4 float) : WXYZ Quaternion that defaults to [1, 0, 0, 0] *optionnal*
 * Scale (array of 3 float) : XYZ Vector that defaults to [1, 1, 1] *optionnal*
 * And array of components, see example below
 
For our example, the JSON file might look like 

```
{
    "entity":
    {
        "name": "Huge transparent falling box",
        "orientation": [0.854, -0.354, 0.146, 0.354],
        "scale": [42, 42, 42],
        
        "components":
        [
            {
                "system": "FancyMeshSystem",
                "mesh": "box.obj",
                "transparent": true
            },
        
            {
                "system": "RigidBodySystem",
                "mass": 1.0
            }
        ]
    }
}

{    
    "entity":
    {
        "name": "Tiny opaque kinematic sphere",
        "position": [42, 42, 42],
        "scale": [0.1, 0.1, 0.1],
        
        "components":
        [
            {
                "system": "FancyMeshSystem",
                "mesh": "sphere.obj",
                "transparent": false
            },
        
            {
                "system": "RigidBodySystem",
                "kinematic": true
            }
        ]
    }
}
```
