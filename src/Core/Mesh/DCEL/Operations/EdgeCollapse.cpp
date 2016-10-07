#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>


namespace Ra {
namespace Core {
namespace DcelOperations {

//v is the optimal vertex to replace the collapsing edge
void edgeCollapse( Dcel& dcel, Index edgeIndex, Vector3 p_result)
{

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

       Il faut supprimer les 3 faces.

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

    //TODO faire pareil avec les full edge !!!
    //-----------------------------------------------


    // Delete the faces
    f1->setHE(NULL);
    if (f2 != NULL) f2->setHE(NULL);

    // Set new position of v1 and delete v2
    //p_result = (v1->P() + v2->P()) / 2;
    v1->setP(p_result);
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
       h=h->Next();
       h->setV(v1);
       h=h->Twin();
    } while (h!=h1 && h!=NULL);

    //Si on a une ouverture dans le maillage, on parcourt à partir de h2 également
    if (h==NULL && h2!=NULL) {
        h=h2->Prev();
        h=h->Twin();
        while (h!=NULL) {
            h->setV(v1);
            h=h->Prev();
            h=h->Twin();
        }
    }

    //On met à jour les twins half-edge
    HalfEdge_ptr e1 = (h1->Prev())->Twin();
    HalfEdge_ptr e2 = (h1->Next())->Twin();
    e1->setTwin(e2);
    e2->setTwin(e1);

    if (h2!=NULL) {
        HalfEdge_ptr e3 = (h2->Prev())->Twin();
        HalfEdge_ptr e4 = (h2->Next())->Twin();
        e3->setTwin(e4);
        e4->setTwin(e3);
    }

    //gérer les half-edges, full-edges et faces

}


}
}
}

//IndexMap< Vertex_ptr >   m_vertex;   // Vertices  Data //changer v1 en v et supprimer v2
//IndexMap< HalfEdge_ptr > m_halfedge; // HalfEdges Data //màj et supp he
//IndexMap< FullEdge_ptr > m_fulledge; // FullEdge  Data //màj et supp fe
//IndexMap< Face_ptr >     m_face;     // Faces     Data //màj et supp f

//Quand on supprime, on met le pointeur à NULL afin de ne pas réaffecter l'index d'un objet supprimé à un nouvel objet
