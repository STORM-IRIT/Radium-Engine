#ifndef NORMAL_DEFINITION
#define NORMAL_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>

namespace Ra {
namespace Core {
namespace Geometry {



//////////////
/// GLOBAL ///
//////////////

/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( normal( face_j ) ) / || sum( normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring.
*/
void uniformNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );



/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( theta_j * normal( face_j ) ) / || sum( theta_j * normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring,
* and theta_j is the angle at v_i in face_j.
*/
void angleWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );



/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( area_j * normal( face_j ) ) / || sum( area_j * normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring,
* and area_j is the area of face_j.
*/
void areaWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );



////////////////
/// ONE RING ///
////////////////

/*
* Return the normalized normal of vertex v, expressed as:
*       sum( normal( face_i ) ) / || sum( normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring.
*/
Vector3 uniformNormal( const Vector3& v, const VectorArray< Vector3 >& p );



/*
* Return the normalized normal of vertex v, expressed as:
*       sum( theta_i * normal( face_i ) ) / || sum( theta_i * normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring,
* and theta_i is the angle at v in face_i.
*/
Vector3 angleWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& p );



/*
* Return the normalized normal of vertex v, expressed as:
*       sum( area_i * normal( face_i ) ) / || sum( area_i * normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring,
* and area_i is the area of face_i.
*/
Vector3 areaWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& p );



}
}
}

#endif // NORMAL_DEFINITION
