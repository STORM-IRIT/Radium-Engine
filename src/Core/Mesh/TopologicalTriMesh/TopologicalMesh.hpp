#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

#include <OpenMesh/Core/Mesh/Traits.hh>

namespace Ra {
namespace Core {


    struct TopologicalMeshTraits : public OpenMesh::DefaultTraits
    {
        typedef OpenMesh::VectorT<Scalar, 3> Point;
        typedef OpenMesh::VectorT<Scalar, 3> Normal;

        VertexAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
        FaceAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
        EdgeAttributes(OpenMesh::Attributes::Status);
        HalfedgeAttributes(OpenMesh::Attributes::Status| OpenMesh::Attributes::Normal);
    };
    typedef OpenMesh::TriMesh_ArrayKernelT<TopologicalMeshTraits> RA_CORE_API TopologicalMesh;

    inline Vector3 convertVec3OpenMeshToEigen(TopologicalMesh::Point vec)
    {
        return Vector3(vec[0],vec[1],vec[2]);
    }
}
}

#endif // TOPOLOGICALMESH_H
