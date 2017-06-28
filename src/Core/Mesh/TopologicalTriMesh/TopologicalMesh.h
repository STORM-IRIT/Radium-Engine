#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include <Core/RaCore.hpp>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Utils/Property.hh>

namespace Ra {
namespace Core {

    struct TopologicalMeshTraits : public OpenMesh::DefaultTraits
    {
      typedef OpenMesh::Vec3f Point;
      typedef OpenMesh::Vec3f Normal;

      VertexAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
      FaceAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
      EdgeAttributes(OpenMesh::Attributes::Status);
      HalfedgeAttributes(OpenMesh::Attributes::Status| OpenMesh::Attributes::Normal);
    };

    typedef OpenMesh::TriMesh_ArrayKernelT<TopologicalMeshTraits> RA_CORE_API TopologicalMesh;
}
}
#endif // TOPOLOGICALMESH_H
