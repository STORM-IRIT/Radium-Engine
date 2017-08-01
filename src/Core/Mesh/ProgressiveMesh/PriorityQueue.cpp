#include "PriorityQueue.hpp"
#include <Core/Log/Log.hpp>
#include <iostream>

namespace Ra
{
    namespace Core
    {

        //------------------------------

        void PriorityQueue::removeEdges(TopologicalMesh::VertexHandle vHandle)
        {
            PriorityQueueData comparator;
            comparator.m_vs = vHandle;
            comparator.m_vt = TopologicalMesh::VertexHandle();

            for (VertexHashContainer::iterator it = m_vertex_hash.find(comparator);
                  it != m_vertex_hash.end();
                  it = m_vertex_hash.find(comparator))
            {
                PriorityQueueData data = *it;
                m_vertex_hash.erase(it);
                m_priority_queue.erase(m_priority_queue.find(data));
                m_vertex_hash.erase(m_vertex_hash.find(data.getSwapped()));
            }
        }

        //------------------------------

        void PriorityQueue::display()
        {
            std::cout << "__________________" << std::endl;
            for(const auto& el : m_priority_queue)
            {
                LOG(logINFO) << "fl_id = "     << (el.m_fl).idx()
                             << ", vs_id = "   << (el.m_vs).idx()
                             << ", vt_id = "   << (el.m_vt).idx()
                             << " et error = " << el.m_err;
            }
        }
    } // namespace Core
} // namespace Ra
