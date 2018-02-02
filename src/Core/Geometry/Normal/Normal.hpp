#ifndef NORMAL_DEFINITION
#define NORMAL_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/Geometry/Adjacency/Adjacency.hpp>

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
void RA_CORE_API uniformNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );


/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( normal( face_j ) ) / || sum( normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring.
*/
void RA_CORE_API uniformNormal(const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T,
                                const std::vector<Index> &duplicateTable, VectorArray< Vector3 >& normal );


/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( normal( face_j ) ) / || sum( normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring.
*/
Vector3 RA_CORE_API localUniformNormal( const uint i, const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, const TVAdj& adj );



/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( theta_j * normal( face_j ) ) / || sum( theta_j * normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring,
* and theta_j is the angle at v_i in face_j.
*
* The definition was taken from:
* "Guide to Computational Geometry Processing"
* [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
* Chapter 8
*/
void RA_CORE_API angleWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );



/*
* Return the normalized normal of vertex v_i, expressed as:
*       sum( area_j * normal( face_j ) ) / || sum( area_j * normal( face_j ) ) ||
*
* where normal( face_j ) is the normalized normal of face_j belonging to v_i one-ring,
* and area_j is the area of face_j.
*/
void RA_CORE_API areaWeightedNormal( const VectorArray< Vector3 >& p, const VectorArray< Triangle >& T, VectorArray< Vector3 >& normal );



////////////////
/// ONE RING ///
////////////////

/*
* Return the normalized normal of vertex v, expressed as:
*       sum( normal( face_i ) ) / || sum( normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring.
*/
Vector3 RA_CORE_API uniformNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring );



/*
* Return the normalized normal of vertex v, expressed as:
*       sum( theta_i * normal( face_i ) ) / || sum( theta_i * normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring,
* and theta_i is the angle at v in face_i.
*
* The definition was taken from:
* "Guide to Computational Geometry Processing"
* [ J. Andreas Baerentzen, Jens Gravesen, Francois Anton, Henrik Aanaes ]
* Chapter 8
*/
Vector3 RA_CORE_API angleWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring );



/*
* Return the normalized normal of vertex v, expressed as:
*       sum( area_i * normal( face_i ) ) / || sum( area_i * normal( face_i ) ) ||
*
* where normal( face_i ) is the normalized normal of face_i belonging to v one-ring,
* and area_i is the area of face_i.
*/
Vector3 RA_CORE_API areaWeightedNormal( const Vector3& v, const VectorArray< Vector3 >& one_ring );



}
}
}

#endif // NORMAL_DEFINITION
