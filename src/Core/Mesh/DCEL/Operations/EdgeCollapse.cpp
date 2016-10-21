#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>


namespace Ra {
namespace Core {
namespace DcelOperations {

short int computeii(Dcel& dcel, Index vsId, Index vtId, Vector3 pResult, Vector3 &vadS, Vector3 &vadL)
{
    Vector3 vtPos = dcel.m_vertex[vtId]->P();
    Vector3 vsPos = dcel.m_vertex[vsId]->P();
    Scalar disVtVp = (vtPos - pResult).squaredNorm();
    Scalar distVmVp = (((vtPos + vsPos) / 2.f) - pResult).squaredNorm();
    Scalar distVsVp = (vsPos - pResult).squaredNorm();
    short int ii = std::min(disVtVp, std::min(distVmVp, distVsVp));
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

ProgressiveMeshData edgeCollapse(Dcel& dcel, Index edgeIndex, Vector3 pResult)
{
    CORE_ASSERT(dcel.m_halfedge[edgeIndex]->V()->idx != dcel.m_halfedge[edgeIndex]->Twin()->V()->idx,"Twins with same starting vertex.");

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


    // Retrieve the edge to collapse
    HalfEdge_ptr edge = dcel.m_halfedge[edgeIndex];

    Vertex_ptr v1 = edge->V();
    Vertex_ptr v2 = edge->Next()->V();

    // Retrieve the two halfedges
    HalfEdge_ptr h1 = edge;
    HalfEdge_ptr h2 = h1->Twin();

    // Retrieve the two faces
    Face_ptr f1 = h1->F();
    Face_ptr f2 = (h2 != NULL) ? h2->F() : nullptr;

    // Data for ProgressiveMeshData
    Vector3 vadS, vadL;
    short int ii = computeii(dcel, v1->idx, v2->idx, pResult, vadS, vadL);
    Vertex_ptr vl = h1->Prev()->V();
    Vertex_ptr vr = h2->Prev()->V();
    Face_ptr flclw = h1->Prev()->Twin()->F();

    // Make the halfEdge of the vertices of the faces
    // to delete point to existing new edges if needed
    if (h1->Prev()->V()->HE()->F() == f1)
        h1->Prev()->V()->setHE(h1->Next()->Twin());
    if (h2->Prev()->V()->HE()->F() == f2)
        h2->Prev()->V()->setHE(h2->Next()->Twin());
    if (v1->HE()->F() == f2)
        v1->setHE(v1->HE()->Twin()->Next());
    else if (v1->HE()->F() == f1)
        v1->setHE(v1->HE()->Prev()->Twin());

    //TODO do the same with full edges !!!
    //-----------------------------------------------


    // Delete the faces
    f1->setHE(NULL);
    if (f2 != NULL) f2->setHE(NULL);

    // Set new position of v1 and delete v2
    v1->setP(pResult);
    v2->setHE(NULL); //on supprime v2

//Dans quel cas se trouve-t-on? Cas classique ou exception?
//    FullEdge_ptr ST = ((h1->Next())->Next())->FE();
//    Vertex_ptr S = ST->V(0);
//    Vertex_ptr T = ST->V(1);
//    if ((h1->Next())->Next())




//Cas classique on où n'est pas sur des bords


//Cas classique
//On ne modifie que les half-edge ayant pour premier vertex v2

    //h pour parcourir les half-edges et remplacer v2 par v1
    //on parcourt à partir de h1
    HalfEdge_ptr h = h1;
    do {
        h = h->Next();
        if (h->idx != h2->idx)
            h->setV(v1);
        h = h->Twin();
    } while (h != h1 && h != NULL);

    //Si on a une ouverture dans le maillage, on parcourt à partir de h2 également
    if (h == NULL && h2 != NULL)
    {
        h = h2->Prev();
        h = h->Twin();
        while (h!=NULL)
        {
            if (h->idx != h2->idx)
                h->setV(v1);
            h = h->Prev();
            h = h->Twin();
        }
    }

    //On met à jour les twins half-edge
    HalfEdge_ptr e1 = (h1->Prev())->Twin();
    HalfEdge_ptr e2 = (h1->Next())->Twin();
    e1->setTwin(e2);
    e2->setTwin(e1);

    if (h2 != NULL)
    {
        HalfEdge_ptr e3 = (h2->Prev())->Twin();
        HalfEdge_ptr e4 = (h2->Next())->Twin();
        e3->setTwin(e4);
        e4->setTwin(e3);
    }

    //gérer les half-edges, full-edges et faces

    // Return ProgressiveMeshData on this edge collapse
    return ProgressiveMeshData(vadL, vadS,
                               edgeIndex, dcel.m_halfedge[edgeIndex]->Twin()->idx,
                               flclw->idx, f1->idx, f2->idx,
                               v1->idx, v2->idx, vl->idx, vr->idx,
                               ii);

}

//------------------------------------------------------------

void edgeCollapse( Dcel& dcel, ProgressiveMeshData pmData)
{
    // compute PResult
    Vector3 vtPos = dcel.m_vertex[pmData.getVtId()]->P();
    Vector3 vsPos = dcel.m_vertex[pmData.getVsId()]->P();
    Vector3 pResult = pmData.computePResult(vtPos, vsPos);

    edgeCollapse(dcel, pmData.getHeFlId(), pResult);
}


} // Dcel Operations
} // Core
} // Ra
