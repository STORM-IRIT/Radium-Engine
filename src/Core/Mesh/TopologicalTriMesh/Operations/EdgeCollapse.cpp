#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeCollapse.hpp>

#include <Core/Log/Log.hpp>


namespace Ra {
namespace Core {
namespace TMOperations {

short int computeii(TopologicalMesh& topologicalMesh, TopologicalMesh::VertexHandle vsHandle, TopologicalMesh::VertexHandle vtHandle, Vector3 pResult, Vector3 &vadS, Vector3 &vadL)
{
    Vector3 vtPos = convertVec3OpenMeshToEigen(topologicalMesh.point(vtHandle));
    Vector3 vsPos = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle));
//    Vector3 vtPos = dcel.m_vertex[vtId]->P();
//    Vector3 vsPos = dcel.m_vertex[vsId]->P();
    Scalar distVtVp = (vtPos - pResult).squaredNorm();
    Scalar distVmVp = (((vtPos + vsPos) / 2.f) - pResult).squaredNorm();
    Scalar distVsVp = (vsPos - pResult).squaredNorm();
    Scalar min = distVtVp;
    short int ii = 0;
    if (min > distVsVp)
    {
        min = distVsVp;
        ii = 1;
    }
    if (min > distVmVp)
    {
        min = distVmVp;
        ii = 2;
    }
    if (ii == 0)
    {
        vadS = vtPos - pResult;
        vadL = vsPos - pResult;
    }
    else if (ii == 1)
    {
        vadS = vsPos - pResult;
        vadL = vtPos - pResult;
    }
    else
    {
        vadS = ((vtPos + vsPos) / 2.f) - pResult;
        vadL = (vtPos - vsPos) / 2.f;
    }
    return ii;
}

//------------------------------------------------------------

ProgressiveMeshData edgeCollapse(TopologicalMesh& topologicalMesh, TopologicalMesh::HalfedgeHandle halfEdgeHandle, Vector3 pResult)
{
    CORE_ASSERT( topologicalMesh.from_vertex_handle(halfEdgeHandle).idx() != topologicalMesh.from_vertex_handle(topologicalMesh.opposite_halfedge_handle(halfEdgeHandle)).idx(),"Twins with same starting vertex.");
//    CORE_ASSERT(dcel.m_halfedge[edgeIndex]->V()->idx != dcel.m_halfedge[edgeIndex]->Twin()->V()->idx,"Twins with same starting vertex.");

    //Exception
    /*
                T
               /|\
              / | \
             /  |  \
            /   S   \
           /   / \   \
          /   /   \   \
         /   /     \   \
        /   /       \   \
       V1 — — — — — — — V2

       We have to delete the 3 faces
    */


    // Retrieve the two halfedges
    TopologicalMesh::HalfedgeHandle h1 = halfEdgeHandle;
    TopologicalMesh::HalfedgeHandle h2 = topologicalMesh.opposite_halfedge_handle(h1);

    TopologicalMesh::VertexHandle v1 = topologicalMesh.from_vertex_handle(h1);
    TopologicalMesh::VertexHandle v2 = topologicalMesh.from_vertex_handle(h2);

    // Retrieve the two faces
    TopologicalMesh::FaceHandle f1 = topologicalMesh.face_handle(h1);
    TopologicalMesh::FaceHandle f2 = topologicalMesh.face_handle(h2);

    TopologicalMesh::VertexHandle vl(-1);
    TopologicalMesh::VertexHandle vr(-1);

    // Data for ProgressiveMeshData
    Vector3 vadS, vadL;
    short int ii = computeii(topologicalMesh, v1, v2, pResult, vadS, vadL);
    if(!topologicalMesh.is_boundary(topologicalMesh.prev_halfedge_handle(h1)))
        vl = topologicalMesh.from_vertex_handle(topologicalMesh.prev_halfedge_handle(h1));
    if(!topologicalMesh.is_boundary(topologicalMesh.prev_halfedge_handle(h2)))
        vr = topologicalMesh.from_vertex_handle(topologicalMesh.prev_halfedge_handle(h2));
    TopologicalMesh::FaceHandle flclw = topologicalMesh.face_handle(topologicalMesh.opposite_halfedge_handle(topologicalMesh.prev_halfedge_handle(h1)));

    topologicalMesh.collapse(h2);

    //Set removed halfedge status
    topologicalMesh.status(h1).set_deleted(true);
    topologicalMesh.status(h2).set_deleted(true);

    topologicalMesh.set_point(v1, TopologicalMesh::Point(pResult.x(),pResult.y(),pResult.z()));

    // Return ProgressiveMeshData on this edge collapse

//    LOG(logINFO) << "Collapse prog_mesh data:";
//    LOG(logINFO) << "edgeIndex : "<< halfEdgeHandle.idx();
//    LOG(logINFO) << "h2.idx() : "<< h2.idx();
//    LOG(logINFO) << "f1.idx() : "<< f1.idx();
//    LOG(logINFO) << "f2.idx() : "<< f2.idx();
//    LOG(logINFO) << "v1.idx() : "<< v1.idx();
//    LOG(logINFO) << "v2.idx() : "<< v2.idx();
//    LOG(logINFO) << "vl.idx() : "<< vl.idx();
//    LOG(logINFO) << "vr.idx() : "<< vr.idx();

    return ProgressiveMeshData(vadL, vadS,
                               halfEdgeHandle, h2,
                               flclw, f1, f2,
                               v1, v2, vl, vr,
                               ii);

}

//------------------------------------------------------------

void edgeCollapse( TopologicalMesh& topologicalMesh, ProgressiveMeshData pmData)
{
    // compute PResult
    Vector3 vtPos = convertVec3OpenMeshToEigen(topologicalMesh.point(pmData.getVt()));
    Vector3 vsPos = convertVec3OpenMeshToEigen(topologicalMesh.point(pmData.getVs()));
    Vector3 pResult = pmData.computePResult(vtPos, vsPos);

    edgeCollapse(topologicalMesh, pmData.getHeFl(), pResult);
}


} // Dcel Operations
} // Core
} // Ra
