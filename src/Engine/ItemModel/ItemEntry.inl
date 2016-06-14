#include "ItemEntry.hpp"

namespace Ra
{
    namespace Engine
    {
        bool ItemEntry::isEntityNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity && !m_component);
        }

        bool ItemEntry::isComponentNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity && m_component && m_roIndex.isInvalid());
        }

        bool ItemEntry::isRoNode() const
        {
            ON_DEBUG(checkConsistency());
            return (m_entity && m_component && m_roIndex.isValid());
        }



        void ItemEntry::checkConsistency() const
        {
            CORE_ASSERT(m_entity || (!m_component && m_roIndex.isInvalid()),
                        "Component or RO  is set while entity is not");
            CORE_ASSERT(m_component || m_roIndex.isInvalid(), "RO  is set while component is not");
        }

        inline bool ItemEntry::operator==( const ItemEntry& rhs ) const
        {
            return m_entity == rhs.m_entity
                && m_component == rhs.m_component
                && m_roIndex == rhs.m_roIndex;
        }



    }
}
