#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <set>
#include <Core/Math/LinearAlgebra.hpp>
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
                Index m_vs_id;
                Index m_vt_id;
                Index m_edge_id;
                Index m_fl_id;
                Scalar m_err;
                Vector3 m_p_result;

                PriorityQueueData() : m_vs_id(-1), m_vt_id(-1), m_edge_id(-1), m_fl_id(-1), m_err(0.f), m_p_result(Vector3::Zero()) {}

                PriorityQueueData(Index vid0, Index vid1, Index eid, Index fid, Scalar error, const Vector3& p)
                        : m_vs_id(vid0), m_vt_id(vid1), m_edge_id(eid), m_fl_id(fid), m_err(error), m_p_result(p) {}

                PriorityQueueData getSwapped()
                {
                    PriorityQueueData data = *this;
                    data.swap();
                    return data;
                }

                inline const PriorityQueueData& swap()
                {
                    std::swap(m_vs_id, m_vt_id);
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
                        if (a1.m_fl_id != a2.m_fl_id)
                            return a1.m_fl_id < a2.m_fl_id;
                        else
                            return a1.m_vs_id + a1.m_vt_id < a2.m_vs_id + a2.m_vt_id;
                    }
                }
            };

            struct comparePriorityQueueDataByVertex
            {
                inline bool operator()(const PriorityQueueData &a1, const PriorityQueueData &a2) const
                {
                    if (a1.m_vs_id != a2.m_vs_id)
                        return a1.m_vs_id < a2.m_vs_id;
                    else
                    {
                        if (a1.m_vt_id == -1 || a2.m_vt_id == -1)
                            return false;
                        else
                            return a1.m_vt_id < a2.m_vt_id;
                    }
                }
            };

            typedef std::set<PriorityQueueData, comparePriorityQueueDataByError> PriorityQueueContainer;
            typedef std::set<PriorityQueueData, comparePriorityQueueDataByVertex> VertexHashContainer;

            /// Insert an element in the priority queue
            void insert(PriorityQueueData item);

            /// Pop the first element
            PriorityQueueData top();

            /// Delete edges having v_id as vertex
            void removeEdges(int v_id);

            void reserve(size_t size);
            bool empty();
            void display();
            int size();


        private:

            PriorityQueueContainer m_priority_queue;
            VertexHashContainer m_vertex_hash;
        };

    } // namespace Ra
} // namespace Core

#include <Core/Mesh/ProgressiveMesh/PriorityQueue.inl>

#endif // PRIORITYQUEUE_H
