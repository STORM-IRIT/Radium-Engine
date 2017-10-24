#include "PriorityQueue.hpp"
#include <Core/Log/Log.hpp>
#include <iostream>

namespace Ra
{
    namespace Core
    {

        //------------------------------

        void PriorityQueue::removeEdges(int v_id)
        {
            PriorityQueueData comparator;
            comparator.m_vs_id = v_id;
            comparator.m_vt_id = -1;

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
                LOG(logINFO) << "fl_id = "     << (el.m_fl_id).getValue()
                             << ", vs_id = "   << (el.m_vs_id).getValue()
                             << ", vt_id = "   << (el.m_vt_id).getValue()
                             << " et error = " << el.m_err;
            }
        }
    } // namespace Core
} // namespace Ra
