#ifndef VERTEXSPLIT_H
#define VERTEXSPLIT_H


#include <Core/RaCore.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Math/LinearAlgebra.hpp>

#include <Core/Mesh/ProgressiveMesh/ProgressiveMeshData.hpp>


namespace Ra {
namespace Core {
namespace TMOperations {

    //void vertexFaceAdjacency(Dcel& dcel, Index vertexIndex, std::vector<Index>& adjOut);
    void createVt(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata);
    void findFlclwNeig(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata,
                       TopologicalMesh::FaceHandle &flclw, TopologicalMesh::FaceHandle &flclwOp, TopologicalMesh::FaceHandle &frcrw, TopologicalMesh::FaceHandle &frcrwOp,
                       std::vector<TopologicalMesh::FaceHandle> adjOut);

    TopologicalMesh::HalfedgeHandle insert_loop(TopologicalMesh& topologicalMesh, TopologicalMesh::HalfedgeHandle _hh,TopologicalMesh::HalfedgeHandle _h1, TopologicalMesh::FaceHandle _f1);
    TopologicalMesh::HalfedgeHandle insert_edge(TopologicalMesh& topologicalMesh, TopologicalMesh::VertexHandle _vh, TopologicalMesh::HalfedgeHandle _h0, TopologicalMesh::HalfedgeHandle _h1,TopologicalMesh::HalfedgeHandle _vtvs,TopologicalMesh::HalfedgeHandle _vsvt);


    RA_CORE_API void vertexSplit(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata);
}
}
}

#endif // VERTEXSPLIT_H
