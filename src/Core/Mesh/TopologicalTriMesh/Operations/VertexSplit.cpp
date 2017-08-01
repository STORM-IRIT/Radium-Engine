#include <Core/Index/Index.hpp>

#include <Core/Mesh/DCEL/HalfEdge.hpp>
#include <Core/Mesh/DCEL/Vertex.hpp>
#include <Core/Mesh/DCEL/FullEdge.hpp>

#include <Core/Mesh/TopologicalTriMesh/Operations/VertexSplit.hpp>

#include <Core/Mesh/DCEL/Iterator/Vertex/VFIterator.hpp>

#include <Core/Log/Log.hpp>


namespace Ra {
namespace Core {

namespace TMOperations {

void createVt(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata)
{
    TopologicalMesh::VertexHandle vsHandle = pmdata.getVs();
    TopologicalMesh::VertexHandle vtHandle = pmdata.getVt();
    Vector3 vtPosition, vsPosition;

    if (pmdata.getii() == 0)
    {
        vtPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVads();
        vsPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVadl();
    }
    else if (pmdata.getii() == 1)
    {
        vtPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVadl();
        vsPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVads();
    }
    else
    {
        vtPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVads() + pmdata.getVadl();
        vsPosition = convertVec3OpenMeshToEigen(topologicalMesh.point(vsHandle)) + pmdata.getVads() - pmdata.getVadl();
    }
    topologicalMesh.set_point(vtHandle, TopologicalMesh::Point(vtPosition.x(),vtPosition.y(),vtPosition.z()));
    topologicalMesh.set_point(vsHandle, TopologicalMesh::Point(vsPosition.x(),vsPosition.y(),vsPosition.z()));

//    if (pmdata.getii() == 0)
//    {
//        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads();
//        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVadl();
//    }
//    else if (pmdata.getii() == 1)
//    {
//        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVadl();
//        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads();
//    }
//    else
//    {
//        vtPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads() + pmdata.getVadl();
//        vsPosition = dcel.m_vertex[vsId]->P() + pmdata.getVads() - pmdata.getVadl();
//    }
//    dcel.m_vertex[vtId]->setP(vtPosition);
//    dcel.m_vertex[vsId]->setP(vsPosition);
}

void findFlclwNeig(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata,
                   TopologicalMesh::FaceHandle &flclw, TopologicalMesh::FaceHandle &flclwOp, TopologicalMesh::FaceHandle &frcrw, TopologicalMesh::FaceHandle &frcrwOp,
                   std::vector<TopologicalMesh::FaceHandle> adjOut)
{
    frcrw = TopologicalMesh::FaceHandle();
    frcrwOp = TopologicalMesh::FaceHandle();
    flclw = TopologicalMesh::FaceHandle();
    flclwOp = TopologicalMesh::FaceHandle();
    TopologicalMesh::HalfedgeHandle he, heCurr;
//    HalfEdge_ptr he, heCurr;

    // Vr
    for (uint i = 0; i < adjOut.size(); i++)
    {
        TopologicalMesh::FaceHandle f = adjOut[i];
        if (pmdata.getVr().idx() != -1 && frcrw.idx() == -1 && frcrwOp.idx() == -1)
        {
            he = topologicalMesh.halfedge_handle(f);
            heCurr.invalidate();
            for (uint j = 0; j < 3; j++)
            {
                if ( topologicalMesh.from_vertex_handle(he) == pmdata.getVr() &&
                    topologicalMesh.to_vertex_handle(he) == pmdata.getVs())
                {
                    heCurr = he;
                    break;
                }
                else
                    he = topologicalMesh.next_halfedge_handle(he);
            }
            if (heCurr.is_valid()) // vrId exists in the face
            {
                frcrw = topologicalMesh.face_handle(he);
                frcrwOp = topologicalMesh.face_handle(topologicalMesh.opposite_halfedge_handle(he));
            }
        }
        if (flclw.idx() == -1 && flclwOp.idx() == -1)
        {
            // Vl
            he = topologicalMesh.halfedge_handle(f);
            heCurr.invalidate();
            for (uint j = 0; j < 3; j++)
            {
                if (topologicalMesh.from_vertex_handle(he) == pmdata.getVl() &&
                    topologicalMesh.to_vertex_handle(he) == pmdata.getVs())
                {
                    heCurr = he;
                    break;
                }
                else
                    he = topologicalMesh.next_halfedge_handle(he);
            }
            if (heCurr.is_valid()) // vlId exists in the face
            {
                flclwOp = topologicalMesh.face_handle(he);
                flclw = topologicalMesh.face_handle(topologicalMesh.opposite_halfedge_handle(he));
            }
        }
        if (flclw.idx() != -1 && flclwOp.idx() != -1 && frcrwOp.idx() != -1 && frcrw.idx() != -1)
            break;
    }

//    // Vr
//    for (uint i = 0; i < adjOut.size(); i++)
//    {
//        Face_ptr f = adjOut[i];
//        if (pmdata.getVrId() != -1 && frcrwId == -1 && frcrwOpId == -1)
//        {
//            he = f->HE();
//            heCurr = nullptr;
//            for (uint j = 0; j < 3; j++)
//            {
//                if (he->V()->idx == pmdata.getVrId() &&
//                    he->Next()->V()->idx == pmdata.getVsId())
//                {
//                    heCurr = he;
//                    break;
//                }
//                else
//                    he = he->Next();
//            }
//            if (heCurr != nullptr) // vrId exists in the face
//            {
//                frcrwId = heCurr->F()->idx;
//                frcrwOpId = heCurr->Twin()->F()->idx;
//            }
//        }
//        if (flclwId == -1 && flclwOpId == -1)
//        {
//            // Vl
//            he = f->HE();
//            heCurr = nullptr;
//            for (uint j = 0; j < 3; j++)
//            {
//                if (he->V()->idx == pmdata.getVlId() &&
//                    he->Next()->V()->idx == pmdata.getVsId())
//                {
//                    heCurr = he;
//                    break;
//                }
//                else
//                    he = he->Next();
//            }
//            if (heCurr != nullptr) // vlId exists in the face
//            {
//                flclwOpId = he->F()->idx;
//                flclwId = he->Twin()->F()->idx;
//            }
//        }
//        if (flclwId != -1 && flclwOpId != -1 && frcrwOpId != -1 && frcrwId)
//            break;
//    }
    CORE_ASSERT(flclwOp.idx() != -1, "PROBLEM FLCLWOP NOT FOUND");
    CORE_ASSERT(flclw.idx() != -1, "PROBLEM FLCLW NOT FOUND");
    CORE_ASSERT(frcrw.idx() != -1, "PROBLEM FRCRW NOT FOUND");
    CORE_ASSERT(frcrwOp.idx() != -1, "PROBLEM FRCRWOP NOT FOUND");
}

void vertexSplit(TopologicalMesh& topologicalMesh, ProgressiveMeshData pmdata)
{
    TopologicalMesh::FaceHandle flclw = pmdata.getFlclw();
    TopologicalMesh::VertexHandle vsHandle = pmdata.getVs();
    TopologicalMesh::VertexHandle vtHandle = pmdata.getVt();
    TopologicalMesh::VertexHandle vlHandle = pmdata.getVr();
    TopologicalMesh::VertexHandle vrHandle = pmdata.getVl();
    TopologicalMesh::FaceHandle flHandle = pmdata.getFr();
    TopologicalMesh::FaceHandle frHandle = pmdata.getFl();

    // Looking for the faces affected by the vertex split
    //std::vector<Index> adjFaces;
    //FaceListvertexFaceAdjacency(dcel, vsId, adjFaces);

    std::vector<TopologicalMesh::FaceHandle> adjFaces;
    TopologicalMesh::VertexFaceIter vf_it = topologicalMesh.vf_iter(vsHandle);
    for(; vf_it.is_valid(); ++vf_it) {
        adjFaces.push_back(vf_it);
    }

//    VFIterator vsfIt = VFIterator(dcel.m_vertex[vsId]);
//    FaceList adjFaces = vsfIt.list();

    //  Looking for faces whose neighboring need to be updated
// TODO : Uncomment 2 lines below ?
    //TopologicalMesh::FaceHandle flclwOp, frcrw, frcrwOp;
    //findFlclwNeig(topologicalMesh, pmdata, flclw, flclwOp, frcrw, frcrwOp, adjFaces);

    // Creating vt, and update vt's and vs's position
    createVt(topologicalMesh, pmdata);

    TopologicalMesh::HalfedgeHandle heFl = pmdata.getHeFl();
    TopologicalMesh::HalfedgeHandle heFr = pmdata.getHeFr();
    TopologicalMesh::HalfedgeHandle v1vl, vlv1, vrv1, v0v1;


    //----------------- TEST -----------------
//    vrv1 = topologicalMesh.find_halfedge(vrHandle, vsHandle);
//    if(!vrv1.is_valid())
//    {
//       TopologicalMesh::VertexOHalfedgeIter voh0_it = topologicalMesh.voh_iter(vsHandle);
//       LOG(logINFO) << "vsId: (" << vsHandle.idx() <<")";
//       for(; voh0_it.is_valid(); ++voh0_it) {
//           LOG(logINFO) << "Edge iter: "<< voh0_it->idx() <<" vertex : "<< (topologicalMesh.to_vertex_handle(voh0_it)).idx() << " Boundary : " << topologicalMesh.is_boundary(voh0_it);
//       }

//       TopologicalMesh::VertexOHalfedgeIter voh1_it = topologicalMesh.voh_iter(vtHandle);
//       LOG(logINFO) << "vtId: (" << vtHandle.idx() <<")";
//       for(; voh1_it.is_valid(); ++voh1_it) {
//           LOG(logINFO) << "Edge iter: "<< voh1_it->idx() <<" vertex : "<< (topologicalMesh.to_vertex_handle(voh1_it)).idx() << " Boundary : " << topologicalMesh.is_boundary(voh0_it);
//       }

//       TopologicalMesh::VertexOHalfedgeIter voh2_it = topologicalMesh.voh_iter(vlHandle);
//       LOG(logINFO) << "vlId: (" << vlHandle.idx() <<")";
//       for(; voh2_it.is_valid(); ++voh2_it) {
//           LOG(logINFO) << "Edge iter: "<< voh2_it->idx() <<" vertex : "<< (topologicalMesh.to_vertex_handle(voh2_it)).idx() << " Boundary : " << topologicalMesh.is_boundary(voh0_it);
//       }

//       TopologicalMesh::VertexOHalfedgeIter voh3_it = topologicalMesh.voh_iter(vrHandle);
//       LOG(logINFO) << "vrId: (" << vrHandle.idx() <<")";
//       for(; voh3_it.is_valid(); ++voh3_it) {
//           LOG(logINFO) << "Edge iter: "<< voh3_it->idx() <<" vertex : "<< (topologicalMesh.to_vertex_handle(voh3_it)).idx() << " Boundary : " << topologicalMesh.is_boundary(voh0_it);
//       }
//    }

  //-------------------------------------------------

    // build loop from halfedge v1->vl
    if (vlHandle.is_valid())
    {
      v1vl = topologicalMesh.find_halfedge(vsHandle, vlHandle);
      assert(v1vl.is_valid());
      vlv1 = insert_loop(topologicalMesh,v1vl,topologicalMesh.prev_halfedge_handle(heFr),flHandle);
    }

    // build loop from halfedge vr->v1
    if (vrHandle.is_valid())
    {
      vrv1 = topologicalMesh.find_halfedge(vrHandle, vsHandle);
      assert(vrv1.is_valid());
      insert_loop(topologicalMesh,vrv1,topologicalMesh.next_halfedge_handle(heFl),frHandle);
    }

    // handle boundary cases
    if (!vlHandle.is_valid())
      vlv1 = topologicalMesh.prev_halfedge_handle(topologicalMesh.halfedge_handle(vsHandle));
    if (!vrHandle.is_valid())
      vrv1 = topologicalMesh.prev_halfedge_handle(topologicalMesh.halfedge_handle(vsHandle));


    // split vertex v1 into edge v0v1
    topologicalMesh.status(heFr).set_deleted(false);
    topologicalMesh.status(heFl).set_deleted(false);
    v0v1 = insert_edge(topologicalMesh,vtHandle, vlv1, vrv1,heFr,heFl);


}


TopologicalMesh::HalfedgeHandle insert_loop(TopologicalMesh& topologicalMesh, TopologicalMesh::HalfedgeHandle _hh,TopologicalMesh::HalfedgeHandle _h1,TopologicalMesh::FaceHandle _f1)
{
  TopologicalMesh::HalfedgeHandle  h0(_hh);
  TopologicalMesh::HalfedgeHandle  o0(topologicalMesh.opposite_halfedge_handle(h0));

  TopologicalMesh::VertexHandle    v0(topologicalMesh.to_vertex_handle(o0));
  TopologicalMesh::VertexHandle    v1(topologicalMesh.to_vertex_handle(h0));

  TopologicalMesh::HalfedgeHandle  h1 = _h1;
  TopologicalMesh::HalfedgeHandle  o1 = topologicalMesh.opposite_halfedge_handle(h1);

  TopologicalMesh::FaceHandle      f0 = topologicalMesh.face_handle(h0);
  TopologicalMesh::FaceHandle      f1 = _f1;

  topologicalMesh.status(h1).set_deleted(false);
  topologicalMesh.status(h0).set_deleted(false);

  // halfedge -> halfedge
  topologicalMesh.set_next_halfedge_handle(topologicalMesh.prev_halfedge_handle(h0), o1);
  topologicalMesh.set_next_halfedge_handle(o1, topologicalMesh.next_halfedge_handle(h0));
  topologicalMesh.set_next_halfedge_handle(h1, h0);
  topologicalMesh.set_next_halfedge_handle(h0, h1);

  // halfedge -> face
  topologicalMesh.set_face_handle(o1, f0);
  topologicalMesh.set_face_handle(h0, f1);
  topologicalMesh.set_face_handle(h1, f1);

  // face -> halfedge
  topologicalMesh.set_halfedge_handle(f1, h0);
  if (f0.is_valid())
    topologicalMesh.set_halfedge_handle(f0, o1);

  // vertex -> halfedge
  topologicalMesh.adjust_outgoing_halfedge(v0);
  topologicalMesh.adjust_outgoing_halfedge(v1);

  topologicalMesh.status(f1).set_deleted(false);

  return h1;
}


TopologicalMesh::HalfedgeHandle insert_edge(TopologicalMesh& topologicalMesh, TopologicalMesh::VertexHandle _vh, TopologicalMesh::HalfedgeHandle _h0, TopologicalMesh::HalfedgeHandle _h1,TopologicalMesh::HalfedgeHandle _vtvs,TopologicalMesh::HalfedgeHandle _vsvt)
{
  assert(_h0.is_valid() && _h1.is_valid());

  TopologicalMesh::VertexHandle  v0 = _vh;
  TopologicalMesh::VertexHandle  v1 = topologicalMesh.to_vertex_handle(_h0);

  assert( v1 == topologicalMesh.to_vertex_handle(_h1));

  TopologicalMesh::HalfedgeHandle v0v1 = _vtvs;
  TopologicalMesh::HalfedgeHandle v1v0 = _vsvt;;

  // vertex -> halfedge
  topologicalMesh.set_halfedge_handle(v0, v0v1);
  topologicalMesh.set_halfedge_handle(v1, v1v0);

  topologicalMesh.status(v0).set_deleted(false);


  // halfedge -> halfedge
  topologicalMesh.set_next_halfedge_handle(v0v1, topologicalMesh.next_halfedge_handle(_h0));
  topologicalMesh.set_next_halfedge_handle(_h0, v0v1);
  topologicalMesh.set_next_halfedge_handle(v1v0, topologicalMesh.next_halfedge_handle(_h1));
  topologicalMesh.set_next_halfedge_handle(_h1, v1v0);


  // halfedge -> vertex
  for (TopologicalMesh::VertexIHalfedgeIter vih_it(topologicalMesh.vih_iter(v0)); vih_it.is_valid(); ++vih_it)
    topologicalMesh.set_vertex_handle(*vih_it, v0);


  // halfedge -> face
  topologicalMesh.set_face_handle(v0v1, topologicalMesh.face_handle(_h0));
  topologicalMesh.set_face_handle(v1v0, topologicalMesh.face_handle(_h1));


  // face -> halfedge
  if (topologicalMesh.face_handle(v0v1).is_valid())
    topologicalMesh.set_halfedge_handle(topologicalMesh.face_handle(v0v1), v0v1);
  if (topologicalMesh.face_handle(v1v0).is_valid())
    topologicalMesh.set_halfedge_handle(topologicalMesh.face_handle(v1v0), v1v0);


  // vertex -> halfedge
  topologicalMesh.adjust_outgoing_halfedge(v0);
  topologicalMesh.adjust_outgoing_halfedge(v1);


  return v0v1;
}



} // Dcel Operations
} // Core
} // Ra
