#ifndef PREFACTOR_DEFINITION
#define PREFACTOR_DEFINITION


#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Animation/Handle/HandleWeight.hpp>

namespace Ra {
namespace Core {
namespace Animation {


// Defining the WeightBlock influencing the position of a vertex
typedef Sparse WeightBlock;

// Defining the PrefactorMatrix containing the prefactorized
typedef Sparse PrefactorMatrix;



/*
* Return the WeightBlock for a given vertex v and weight w.
* The block is in the form:
*       Xij = [ w v_x I   |   w v_y I   |   w v_z I   |   w I ]
* where I is the 3x3 Identity matrix and Xij is a 3x12 matrix.
*
* The definition was taken from:
* "Skinning: Real-Time Shape Deformation"
* [ Alec Jacobson, Zhigang Deng, Ladislav Kavan, J. P. Lewis ]
* SIGGRAPH Asia 2014
*/
WeightBlock computeBlockXij( const Vector3& v, const Scalar& w );


/*
* Return the PrefactorMatrix for the vertices of a mesh and their skinning weights.
* The matrix is in the form:
*       X = [ X_00 |   ...   | X_0m ]
*           [  ... |   ...   |  ... ]
*           [ X_n0 |   ...   | X_nm ]
* where X_ij is the 3x12 WeightBlock for vertex i and handle j, X is a 3nx12m matrix,
* n is the number of vertices and m is the handle_size ( number of handles ).
*
* For direct skinning methods the new position of vertex v is computed as:
*       v' = Xt
* where v' is the new position, X is the PrefactorMatrix containing
*       weight * vertex
* and t is a 12m vector containing the handles vectorized transform matrices.
*
* The definition was taken from:
* "Skinning: Real-Time Shape Deformation"
* [ Alec Jacobson, Zhigang Deng, Ladislav Kavan, J. P. Lewis ]
* SIGGRAPH Asia 2014
*/
PrefactorMatrix computeMatrixX( const VectorArray< Vector3 >& v, const MeshWeight& weight, const uint handle_size );



} // namespace Animation
} // Namespace Core
} // Namespace Ra


#endif // PREFACTOR_DEFINITION

