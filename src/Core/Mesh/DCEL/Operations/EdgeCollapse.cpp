#include <Core/Mesh/DCEL/Operations/EdgeCollapse.hpp>
#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>


namespace Ra {
namespace Core {
namespace DcelOperations {

//v is the optimal vertex to replace the collapsing edge
void edgeCollapse( Dcel& dcel, Index edgeIndex /*,Vector3d v*/ ) //v=v1+v2/2
{

    //On récupère le edge à collapser
    FullEdge_ptr edge = dcel.m_fulledge[edgeIndex];

    Vertex_ptr v1 = edge->V( 0 );
    Vertex_ptr v2 = edge->V( 1 );

    Vector3 v1_c= v1->P();
    Vector3 v2_c= v2->P();
    Vector3 v = (v1_c+v2_c)/2;

    v1->setP(v);


    //On récupère les 2 half-edge
    HalfEdge_ptr h1 = edge->HE( 0 );
    HalfEdge_ptr h2 = h1->Twin();

//Cas classique on où n'est pas sur des bords
//On ne modifie que les half-edge ayant pour premier vertex v2

    //h pour parcourir les half-edges et remplacer v2 par v1
    HalfEdge_ptr h = h1->Next();

    while (h!=h2) {
        h->setV(v1);
        h=h->Twin();
        h=h->Next();
    }

    //On met à jour les twins half-edge
    HalfEdge_ptr e1 = (h1->Prev())->Twin();
    HalfEdge_ptr e2 = (h1->Next())->Twin();
    e1.setTwin(e2);
    e2.setTwin(e1);
    HalfEdge_ptr e3 = (h2->Prev())->Twin();
    HalfEdge_ptr e4 = (h2->Next())->Twin();
    e3.setTwin(e4);
    e4.setTwin(e3);

    //gérer les half-edges, full-edges et faces



//    HalfEdge_ptr h = h1->Next();
//    // Attention 2 cas : cas cyclique et non-cyclique (x2)
//    // Cas cyclique
//    while( h != h2 ) {
//        //on remplace le premier vertex par v
//        h->setV(v);
//        h=h->Twin();
//        //on se sert du twin pour changer de face
//        h=h->Next();
//    }
//    while( h != h1 ) {
//        //on remplace le premier vertex par v
//        h->setV(v);
//        h=h->Twin();
//        //on se sert du twin pour changer de face
//        h=h->Next();
//    }

//    //mettre à jour les 2 nouveaux couples de twins aux extrémités
//    if (h==h1) {
//        //couple 1
//        HalfEdge_ptr e1 = (h1->Prev())->Twin();
//        HalfEdge_ptr e2 = (h1->Next())->Twin(); //à faire en double
//        e1.setTwin(e2);
//        //couple 2
//        HalfEdge_ptr e3 = (h2->Next())->Twin();
//        HalfEdge_ptr e4 = (h2->Prev())->Twin();
//        e3.setTwin(e4);
//    }
    // Cas non cyclique
//    if (h==NULL) {
//        h = h1->Prev();
//        h = h->Twin();
//        while (h!=NULL) {
//            h->setV(v);
//            h=h->Twin();
//            h=h->Next();
//        }
//        h = h2->Next();
//        while (h!=NULL) {
//            h->setV(v);
//            h=h->Twin();
//            h=h->Next();
//        }
//        //màj extrémités
//        HalfEdge_ptr e5 = (h1->Prev())->Twin();
//        HalfEdge_ptr e6 = (h1->Next())->Twin();
//        e5.setTwin(e6);
//    }
}

}
}
}

//IndexMap< Vertex_ptr >   m_vertex;   // Vertices  Data //changer v1 en v et supprimer v2
//IndexMap< HalfEdge_ptr > m_halfedge; // HalfEdges Data //màj et supp he
//IndexMap< FullEdge_ptr > m_fulledge; // FullEdge  Data //màj et supp fe
//IndexMap< Face_ptr >     m_face;     // Faces     Data //màj et supp f

//Quand on supprime, on met le pointeur à NULL afin de ne pas réaffecter l'index d'un objet supprimé à un nouvel objet
