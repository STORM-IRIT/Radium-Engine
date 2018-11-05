 # Position-based Skinning (PBS) Plugin Manual

## Usage

The RadiumEngine's `Skinning Plugin` provides a list of different skinning methods: 
*Linear blend skinning  *Dual quaternion skinning  *Optimized Centers of Rotation *Position-based skinning

The `Position-based skinning` Plugin enables to enhance the skinning process by adding dynamics behaviour to the skinning deformation.

To be able to use this Plugin, a tetrahedral mesh of the surface mesh is required:
(please bear in mind to generate tetrahedral mesh that preserve the outer vertices of the surface mesh,
 and the # of the tetrahedral elements not higher than 9K, otherwise the animation will be slow, and you will observe some artifacts).

When you load the Plugin from the skinning list, the system searches for a corresponding tetrahedral mesh of the already loaded surface mesh:
**if the tetrahedral mesh exists in the directory, then skinning can be performed and the user is provided with a set of stiffness parameters in which you can tune.
**if the tetrahedral mesh does not existed in the directory, then you can not simply progress more, and you receive error an required you to load tetrahedral mesh. 


![Screenshot](https://github.com/NadineAB/Radium-Engine/tree/master/Docs/master/PBS.png)