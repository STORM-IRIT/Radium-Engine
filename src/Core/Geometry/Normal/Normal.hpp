#ifndef NORMAL_DEFINITION
#define NORMAL_DEFINITION

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/VectorArray.hpp>

namespace Ra {
namespace Core {
namespace Geometry {



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
Vector3 angleWeigthedNormal( const Vector3& v, const VectorArray< Vector3 >& p );



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
