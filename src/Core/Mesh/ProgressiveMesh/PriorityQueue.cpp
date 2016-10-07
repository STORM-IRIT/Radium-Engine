#include "PriorityQueue.hpp"
#include <Core/Log/Log.hpp>
#include <iostream>

namespace Ra
{
    namespace Core
    {


        void PriorityQueue::insert(PriorityQueueData item)
        {
            std::pair<PriorityQueueContainer::iterator,bool> pair = m_priority_queue.insert(item);
            if(!(bool)pair.second)
            {
                return; // temporary fix
            }
            m_vertex_hash.insert(item);
            m_vertex_hash.insert(item.getSwapped());
        }

        //------------------------------

        PriorityQueue::PriorityQueueData PriorityQueue::top()
        {
            PriorityQueueContainer::iterator it_priority_queue = m_priority_queue.begin();
            PriorityQueue::PriorityQueueData data = *it_priority_queue;
            m_vertex_hash.erase(m_vertex_hash.find(data));
            m_vertex_hash.erase(m_vertex_hash.find(data.getSwapped()));
            m_priority_queue.erase(it_priority_queue);
            return data;
        }

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

        bool PriorityQueue::empty()
        {
            return size() <= 0;
        }

        //------------------------------

        int PriorityQueue::size()
        {
            return m_priority_queue.size();
        }

        //------------------------------

        void PriorityQueue::display()
        {
            PriorityQueueContainer::iterator it = m_priority_queue.begin();
            std::cout << "__________________" << std::endl;
            for (it = m_priority_queue.begin(); it != m_priority_queue.end(); it++)
            {
                LOG(logINFO) << "fl_id = " << ((*it).m_fl_id).getValue() << ", vs_id = " << ((*it).m_vs_id).getValue() << ", vt_id = " << ((*it).m_vt_id).getValue() << " et error = " << (*it).m_err;
            }
        }
    } // namespace Core
} // namespace Ra
