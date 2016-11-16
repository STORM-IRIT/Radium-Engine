#include "PriorityQueue.hpp"
#include <Core/Log/Log.hpp>

namespace Ra
{
    namespace Core
    {

        inline void PriorityQueue::insert(PriorityQueueData item)
        {
            std::pair<PriorityQueueContainer::iterator,bool> pair = m_priority_queue.insert(item);
            if(!(bool)pair.second)
            {
                LOG(logINFO) << "pb bad insert in priority queue : " << item.m_vs_id << ", " << item.m_vt_id;
                return; // temporary fix
            }
            m_vertex_hash.insert(item);
            m_vertex_hash.insert(item.getSwapped());
        }

        //------------------------------

        inline PriorityQueue::PriorityQueueData PriorityQueue::top()
        {

            CORE_ASSERT(! m_priority_queue.empty(), "Invalid reference vertex");

            PriorityQueueContainer::iterator it_priority_queue;
            PriorityQueue::PriorityQueueData data;

            it_priority_queue = m_priority_queue.begin();
            data = *it_priority_queue;

            m_priority_queue.erase(it_priority_queue);
            m_vertex_hash.erase(m_vertex_hash.find(data));
            m_vertex_hash.erase(m_vertex_hash.find(data.getSwapped()));
            return data;
        }


        inline void PriorityQueue::reserve(size_t size)
        {
           // m_vertex_hash.get_allocator().allocate(2*size);
           //m_priority_queue.get_allocator().allocate(size);
        }


        //------------------------------

        inline bool PriorityQueue::empty()
        {
            return size() <= 0;
        }

        //------------------------------

        inline int PriorityQueue::size()
        {
            return m_priority_queue.size();
        }

    }

}
