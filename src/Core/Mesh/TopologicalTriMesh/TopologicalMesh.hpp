#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

#include <OpenMesh/Core/Mesh/Traits.hh>

namespace Ra {
namespace Core {


    //Define an OpenMesh TriMesh structure according to http://openmesh.org/Documentation/OpenMesh-2.1-Documentation/mesh_type.html
    // Attributes define data store on structure.
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

    inline Eigen::Map<const Vector3> convertVec3OpenMeshToEigen(const TopologicalMesh::Point &vec)
    {
        return Eigen::Map<const Vector3>(vec.data());
    }
}
}

#endif // TOPOLOGICALMESH_H
