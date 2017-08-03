#include <Core/Mesh/TopologicalTriMesh/Operations/EdgeCollapse.hpp>

//#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
//#include <Core/Mesh/DCEL/HalfEdge.hpp>
//#include <Core/Mesh/DCEL/Vertex.hpp>
//#include <Core/Mesh/DCEL/FullEdge.hpp>
//#include <Core/Mesh/DCEL/Iterator/Vertex/VHEIterator.hpp>

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


//    // Retrieve the edge to collapse
//    HalfEdge_ptr edge = dcel.m_halfedge[edgeIndex];

//    Vertex_ptr v1 = edge->V();
//    Vertex_ptr v2 = edge->Next()->V();

//    // Retrieve the two halfedges
//    HalfEdge_ptr h1 = edge;
//    HalfEdge_ptr h2 = h1->Twin();

//    // Retrieve the two faces
//    Face_ptr f1 = h1->F();
//    Face_ptr f2 = (h2 != NULL) ? h2->F() : nullptr;

//    // Data for ProgressiveMeshData
//    Vector3 vadS, vadL;
//    short int ii = computeii(dcel, v1->idx, v2->idx, pResult, vadS, vadL);
//    Vertex_ptr vl = h1->Prev()->V();
//    Vertex_ptr vr = h2->Prev()->V();
//    Face_ptr flclw = h1->Prev()->Twin()->F();


//    // Make the halfEdge of the vertices of the faces
//    // to delete point to existing new edges if needed
//    if (h1->Prev()->V()->HE()->F() == f1)
//        h1->Prev()->V()->setHE(h1->Next()->Twin());
//    if (h2->Prev()->V()->HE()->F() == f2)
//        h2->Prev()->V()->setHE(h2->Next()->Twin());
//    if (v1->HE()->F() == f2)
//        v1->setHE(v1->HE()->Twin()->Next());
//    else if (v1->HE()->F() == f1)
//        v1->setHE(v1->HE()->Prev()->Twin());

//    //-----------------------------------------------
//    // TODO do the same with full edges !!!
//    //-----------------------------------------------
//    // TODO do something for mesh with holes
//    //-----------------------------------------------

//    // Delete the faces
//    f1->setHE(NULL);
//    if (f2 != NULL) f2->setHE(NULL);

//    VHEIterator vIt = VHEIterator(v2);
//    HalfEdgeList adjHE = vIt.list();
//    for (uint i = 0; i < adjHE.size(); i++)
//    {
//        adjHE[i]->setV(v1);
//    }

//    // Set new position of v1 and delete v2
//    v1->setP(pResult);
//    v2->setHE(NULL);

//    // Updating twins
//    HalfEdge_ptr e1 = (h1->Prev())->Twin();
//    HalfEdge_ptr e2 = (h1->Next())->Twin();
//    e1->setTwin(e2);
//    e2->setTwin(e1);
//    if (h2 != NULL)
//    {
//        HalfEdge_ptr e3 = (h2->Prev())->Twin();
//        HalfEdge_ptr e4 = (h2->Next())->Twin();
//        e3->setTwin(e4);
//        e4->setTwin(e3);
//    }

    //Collapse h2 (v2 mark as deleted) -> move v1 to pResult -> destroy v2 (mesh reevaluation)
    TopologicalMesh::HalfedgeHandle nexth1 = topologicalMesh.next_halfedge_handle(h1);
    TopologicalMesh::HalfedgeHandle prevh1 = topologicalMesh.prev_halfedge_handle(h1);

    TopologicalMesh::HalfedgeHandle nexth2 = topologicalMesh.next_halfedge_handle(h2);
    TopologicalMesh::HalfedgeHandle prevh2 = topologicalMesh.prev_halfedge_handle(h2);

    topologicalMesh.collapse(h2);

    //Set removed halfedge status
    topologicalMesh.status(h1).set_deleted(true);
    topologicalMesh.status(nexth1).set_deleted(true);
    topologicalMesh.status(prevh1).set_deleted(true);

    topologicalMesh.status(h2).set_deleted(true);
    topologicalMesh.status(nexth2).set_deleted(true);
    topologicalMesh.status(prevh2).set_deleted(true);

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
//    // compute PResult
//    Vector3 vtPos = dcel.m_vertex[pmData.getVtId()]->P();
//    Vector3 vsPos = dcel.m_vertex[pmData.getVsId()]->P();
//    Vector3 pResult = pmData.computePResult(vtPos, vsPos);

//    edgeCollapse(dcel, pmData.getHeFlId(), pResult);
}


} // Dcel Operations
} // Core
} // Ra
