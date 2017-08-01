#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <set>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/TopologicalTriMesh/TopologicalMesh.hpp>
#include <Core/Index/Index.hpp>

namespace Ra
{
    namespace Core
    {

        class PriorityQueue
        {
        public:

            struct PriorityQueueData
            {
                TopologicalMesh::VertexHandle m_vs;
                TopologicalMesh::VertexHandle m_vt;
                TopologicalMesh::HalfedgeHandle m_edge;
                TopologicalMesh::FaceHandle m_fl;
                Scalar m_err;
                Vector3 m_p_result;
                int m_index;


                PriorityQueueData() : m_vs(TopologicalMesh::VertexHandle()), m_vt(TopologicalMesh::VertexHandle()), m_edge(TopologicalMesh::HalfedgeHandle()), m_fl(TopologicalMesh::FaceHandle()), m_err(0.f), m_p_result(Vector3::Zero()), m_index(-1) {}

                PriorityQueueData(TopologicalMesh::VertexHandle vid0, TopologicalMesh::VertexHandle vid1, TopologicalMesh::HalfedgeHandle eid, TopologicalMesh::FaceHandle fid, Scalar error, const Vector3& p, int idx)
                        : m_vs(vid0), m_vt(vid1), m_edge(eid), m_fl(fid), m_err(error), m_p_result(p), m_index(idx) {}

                PriorityQueueData getSwapped()
                {
                    PriorityQueueData data = *this;
                    data.swap();
                    return data;
                }

                inline const PriorityQueueData& swap()
                {
                    std::swap(m_vs, m_vt);
                    return *this;
                }
            };


            struct comparePriorityQueueDataByError
            {
                inline bool operator()(const PriorityQueueData &a1, const PriorityQueueData &a2) const
                {
                    if (a1.m_err != a2.m_err)
                        return a1.m_err < a2.m_err;
                    else
                    {
                        if (a1.m_fl.idx() != a2.m_fl.idx())
                            return a1.m_fl.idx() < a2.m_fl.idx();
                        else
                            return a1.m_vs.idx() + a1.m_vt.idx() < a2.m_vs.idx() + a2.m_vt.idx();
                    }
                }
            };

//            struct comparePriorityQueueDataByError
//            {
//                inline bool operator()(const PriorityQueueData &a1, const PriorityQueueData &a2)
//                {
//                    if (a1.m_err != a2.m_err)
//                    {
//                        return a1.m_err < a2.m_err;
//                    }
//                    else
//                    {
//                        if (a1.m_fl_id != a2.m_fl_id)
//                        {
//                            return a1.m_fl_id < a2.m_fl_id;
//                        }
//                        else
//                        {
//                            if (a1.m_vs_id != a2.m_vs_id)
//                            {
//                                return a1.m_vs_id < a2.m_vs_id; }
//                            else
//                            {
//                                return a1.m_vt_id < a2.m_vt_id; }
//                        }
//                    }
//                }
//            };

            struct comparePriorityQueueDataByVertex
            {
                inline bool operator()(const PriorityQueueData &a1, const PriorityQueueData &a2) const
                {
                    if (a1.m_vs.idx() != a2.m_vs.idx())
                        return a1.m_vs.idx() < a2.m_vs.idx();
                    else
                    {
                        if (a1.m_vt.idx() == -1 || a2.m_vt.idx() == -1)
                            return false;
                        else
                            return a1.m_vt.idx() < a2.m_vt.idx();
                    }
                }
            };

            typedef std::set<PriorityQueueData, comparePriorityQueueDataByError> PriorityQueueContainer;
            typedef std::set<PriorityQueueData, comparePriorityQueueDataByVertex> VertexHashContainer;

            /// Insert an element in the priority queue
            void insert(PriorityQueueData item);

            /// Pop the first element
            PriorityQueueData top();


            ///Return the first element
            PriorityQueueData firstData();

            /// Delete edges having v_id as vertex
            void removeEdges(TopologicalMesh::VertexHandle vHandle);

            void reserve(size_t size);
            bool empty();
            void display();
            int size();

//            int getIndex();
//            void setIndex(int idx);

            PriorityQueueContainer getPriorityQueueContainer();


        private:

            PriorityQueueContainer m_priority_queue;
            VertexHashContainer m_vertex_hash;

            //int m_index;
        };

    } // namespace Ra
} // namespace Core

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.inl>

#endif // PRIORITYQUEUE_H
