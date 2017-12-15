#ifndef RADIUMENGINE_POINT_CLOUD_HPP_
#define RADIUMENGINE_POINT_CLOUD_HPP_

#include <Core/RaCore.hpp>

#include <Eigen/Eigenvalues>

#include <Core/Math/Math.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Math/Obb.hpp>
#include <Core/Containers/VectorArray.hpp>

namespace Ra
{
    namespace Core
    {
        /// This file contains functions operating on any unstructured set of points.
        /// If not stated otherwise the functions behaviour is undefined if the set
        /// of points is empty.
        namespace PointCloud
        {
            /// Compute the mean point of a set of points, i.e. the barycenter.
            RA_CORE_API inline Vector3 meanPoint(const Vector3Array& pts);

            /// Returns a transform computed by PCA of the given set of points.
            /// The rotation gives you the principal directions in increasing
            /// order of importance (Z = principal direction)
            /// The translation is the barycenter of the point set.
            RA_CORE_API inline Transform principalAxis(const Vector3Array& pts);

            /// Returns the axis-aligned bounding box of a set of points.
            /// This function returns an empty AABB if the set of points is
            /// empty.
            RA_CORE_API inline Aabb aabb(const Vector3Array& pts);

            /// Computes an oriented bounding box based on PCA of the points coordinates.
            RA_CORE_API inline Obb pcaObb(const Vector3Array& pts);

        }
    }
}

#include <Core/Geometry/PointCloud/PointCloud.inl>

#endif // RADIUMENGINE_POINT_CLOUD_HPP_
