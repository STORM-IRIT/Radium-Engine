#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/RaCore.hpp>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

#include <OpenMesh/Core/Mesh/Traits.hh>

namespace Ra
{
namespace Core
{

///\class TopoVector3 : small extension to Vector3 for OpenMesh compatibility
class TopoVector3 : public Ra::Core::Vector3
{
  public:
    using Ra::Core::Vector3::Vector3;
    Scalar length() { return norm(); }
    TopoVector3 vectorize(Scalar v)
    {
        (*this)[0] = v;
        (*this)[1] = v;
        (*this)[2] = v;
        return *this;
    }
    TopoVector3 normalize()
    {
        Ra::Core::Vector3::normalize();
        return *this;
    }
};

Scalar dot(TopoVector3 a, TopoVector3 b);
TopoVector3 cross(TopoVector3 a, TopoVector3 b);
}
}

template <>
struct OpenMesh::vector_traits<Ra::Core::TopoVector3>
{
    /// Type of the vector class
    typedef typename Ra::Core::Vector3 vector_type;

    /// Type of the scalar value
    typedef Scalar value_type;

    /// size/dimension of the vector
    static const size_t size_ = 3;

    /// size/dimension of the vector
    static size_t size() { return size_; }
};

namespace Ra
{
namespace Core
{

// Define an OpenMesh TriMesh structure according to
// http://openmesh.org/Documentation/OpenMesh-2.1-Documentation/mesh_type.html
    // Attributes define data store on structure.

struct TopologicalMeshTraits : public OpenMesh::DefaultTraits
{
    typedef TopoVector3 Point;
    typedef TopoVector3 Normal;

        VertexAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
        FaceAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
        EdgeAttributes(OpenMesh::Attributes::Status);
        HalfedgeAttributes(OpenMesh::Attributes::Status| OpenMesh::Attributes::Normal);

  public:
    };

class RA_CORE_API TopologicalMesh : public OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>
{
    using base = OpenMesh::PolyMesh_ArrayKernelT<TopologicalMeshTraits>;
    using base::PolyMesh_ArrayKernelT;
};
}
}

#endif // TOPOLOGICALMESH_H
