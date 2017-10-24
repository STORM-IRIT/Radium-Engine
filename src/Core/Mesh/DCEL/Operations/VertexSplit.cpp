#include <Core/Index/Index.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Mesh/DCEL/Operations/VertexSplit.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Log/Log.hpp>


namespace Ra {
namespace Core {

namespace DcelOperations {

void createVt(Dcel& dcel, ProgressiveMeshData pmdata)
{
    Index vsId = pmdata.getVsId();
    Index vtId = pmdata.getVtId();
    Vector3 vtPosition, vsPosition;
    if (pmdata.getii() == 0)
    {
        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads();
        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVadl();
    }
    else if (pmdata.getii() == 1)
    {
        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVadl();
        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads();
    }
    else
    {
        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads() + pmdata.getVadl();
        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads() - pmdata.getVadl();
    }
    dcel.m_vertex[vtId]->setP(vtPosition);
    dcel.m_vertex[vsId]->setP(vsPosition);
}

void findFlclwNeig(Dcel& dcel, ProgressiveMeshData pmdata,
                   Index &flclwId, Index &flclwOpId, Index &frcrwId, Index &frcrwOpId,
                   FaceList adjOut)
{
    frcrwId = -1;
    frcrwOpId = -1;
    flclwId = -1;
    flclwOpId = -1;
    HalfEdge_ptr he, heCurr;

    // Vr
    for (uint i = 0; i < adjOut.size(); i++)
    {
        Face_ptr f = adjOut[i];
        if (pmdata.getVrId() != -1 && frcrwId == -1 && frcrwOpId == -1)
        {
            he = f->HE();
            heCurr = nullptr;
            for (uint j = 0; j < 3; j++)
            {
                if (he->V()->idx == pmdata.getVrId() &&
                    he->Next()->V()->idx == pmdata.getVsId())
                {
                    heCurr = he;
                    break;
                }
                else
                    he = he->Next();
            }
            if (heCurr != nullptr) // vrId exists in the face
            {
                frcrwId = heCurr->F()->idx;
                frcrwOpId = heCurr->Twin()->F()->idx;
            }
        }
        if (flclwId == -1 && flclwOpId == -1)
        {
            // Vl
            he = f->HE();
            heCurr = nullptr;
            for (uint j = 0; j < 3; j++)
            {
                if (he->V()->idx == pmdata.getVlId() &&
                    he->Next()->V()->idx == pmdata.getVsId())
                {
                    heCurr = he;
                    break;
                }
                else
                    he = he->Next();
            }
            if (heCurr != nullptr) // vlId exists in the face
            {
                flclwOpId = he->F()->idx;
                flclwId = he->Twin()->F()->idx;
            }
        }
        if (flclwId != -1 && flclwOpId != -1 && frcrwOpId != -1 && frcrwId)
            break;
    }
    CORE_ASSERT(flclwOpId != -1, "PROBLEM FLCLWOP NOT FOUND");
    CORE_ASSERT(flclwId != -1, "PROBLEM FLCLW NOT FOUND");
    CORE_ASSERT(frcrwId != -1, "PROBLEM FRCRW NOT FOUND");
    CORE_ASSERT(frcrwOpId != -1, "PROBLEM FRCRWOP NOT FOUND");
}

void vertexSplit(Dcel& dcel, ProgressiveMeshData pmdata)
{
    Index flclwId = pmdata.getFlclwId();
    Index vsId = pmdata.getVsId();
    Index vtId = pmdata.getVtId();
    Index vlId = pmdata.getVlId();
    Index vrId = pmdata.getVrId();
    Index flId = pmdata.getFlId();
    Index frId = pmdata.getFrId();

    // Looking for the faces affected by the vertex split
    //std::vector<Index> adjFaces;
    //FaceListvertexFaceAdjacency(dcel, vsId, adjFaces);

    VFIterator vsfIt = VFIterator(dcel.m_vertex[vsId]);
    FaceList adjFaces = vsfIt.list();

    //  Looking for faces whose neighboring need to be updated
    Index flclwOpId, frcrwId, frcrwOpId;
    findFlclwNeig(dcel, pmdata, flclwId, flclwOpId, frcrwId, frcrwOpId, adjFaces);

    // Creating vt, and update vt's and vs's position
    createVt(dcel, pmdata);
    dcel.m_vertex[vtId]->setHE(dcel.m_halfedge[pmdata.getHeFrId()]);

    // Get Halfedges of affected faces
    HalfEdge_ptr heFlclw    =      dcel.m_face[flclwId]->HE();
    HalfEdge_ptr heFlclwOp  =    dcel.m_face[flclwOpId]->HE();
    HalfEdge_ptr heFrcrw    =      dcel.m_face[frcrwId]->HE();
    HalfEdge_ptr heFrcrwOp  =    dcel.m_face[frcrwOpId]->HE();
    HalfEdge_ptr heFlclwCurr, heFlclwOpCurr, heFrcrwCurr, heFrcrwOpCurr;
    for (uint i = 0; i < 3; i++)
    {
        if (heFlclw->V()->idx == vsId) heFlclwCurr = heFlclw;
        else heFlclw = heFlclw->Next();

        if (heFlclwOp->Next()->V()->idx == vsId) heFlclwOpCurr = heFlclwOp;
        else heFlclwOp = heFlclwOp->Next();

        if (heFrcrw->Next()->V()->idx == vsId) heFrcrwCurr = heFrcrw;
        else heFrcrw = heFrcrw->Next();

        if (heFrcrwOp->V()->idx == vsId) heFrcrwOpCurr = heFrcrwOp;
        else heFrcrwOp = heFrcrwOp->Next();
    }

    // Update Fl and Fr halfedges
    HalfEdge_ptr heFl = dcel.m_halfedge[pmdata.getHeFlId()];
    HalfEdge_ptr heFlPrev = heFl->Prev();
    HalfEdge_ptr heFlNext = heFl->Next();
    HalfEdge_ptr heFr = dcel.m_halfedge[pmdata.getHeFrId()];
    HalfEdge_ptr heFrPrev = heFr->Prev();
    HalfEdge_ptr heFrNext = heFr->Next();

    heFl->setV(dcel.m_vertex[vsId]);
    heFr->setV(dcel.m_vertex[vtId]);
    heFl->setTwin(heFr);
    heFr->setTwin(heFl);
    heFl->FE()->setHE(heFl);
    heFr->FE()->setHE(heFr);
    heFl->setF(dcel.m_face[flId]);
    heFr->setF(dcel.m_face[frId]);

    // Update Neighbors
    // Fl
    heFlNext->setV(dcel.m_vertex[vtId]);
    heFlNext->setF(dcel.m_face[flId]);
    heFlNext->FE()->setHE(heFlNext);
    heFlNext->setTwin(heFlclwOpCurr);
    heFlclwOpCurr->setTwin(heFlNext);
    heFlclwOpCurr->Next()->setV(dcel.m_vertex[vtId]);

    heFlPrev->setV(dcel.m_vertex[vlId]);
    heFlPrev->setF(dcel.m_face[flId]);
    heFlPrev->FE()->setHE(heFlPrev);
    heFlPrev->setTwin(heFlclwCurr);
    heFlclwCurr->setTwin(heFlPrev);

    // Fr
    heFrNext->setV(dcel.m_vertex[vsId]);
    heFrNext->setF(dcel.m_face[frId]);
    heFrNext->FE()->setHE(heFrNext);
    heFrNext->setTwin(heFrcrwCurr);
    heFrcrwCurr->setTwin(heFrNext);

    heFrPrev->setV(dcel.m_vertex[vrId]);
    heFrPrev->setF(dcel.m_face[frId]);
    heFrPrev->FE()->setHE(heFrPrev);
    heFrPrev->setTwin(heFrcrwOpCurr);
    heFrcrwOpCurr->setTwin(heFrPrev);
    heFrcrwOpCurr->setV(dcel.m_vertex[vtId]);

    // Vs becomes Vt in some faces
    HalfEdge_ptr he = heFlclwOpCurr->Next();
    he = he->Twin()->Next();
    while (he != heFrcrwOpCurr)
    {
        he->setV(dcel.m_vertex[vtId]);
        he = he->Twin()->Next();
    }

    // Set HE of Vs and Vt
    Vertex_ptr vs = dcel.m_vertex[vsId];
    Vertex_ptr vt = dcel.m_vertex[vtId];
    vs->setHE(heFl);
    vt->setHE(heFr);

    // Update Fl and Fr
    dcel.m_face[pmdata.getFlId()]->setHE(heFl);
    dcel.m_face[pmdata.getFrId()]->setHE(heFr);
}


} // Dcel Operations
} // Core
} // Ra
