#include <Core/Index/Index.hpp>
#include <Core/Index/IndexMap.hpp>
namespace Ra
{
    namespace Core
    {

        /// CONSTRUCTOR
        Index::Index ( const int i )
        {
            m_idx = ( i < 0 ) ? s_invalid : i;
        }
        Index::Index ( const Index& i )
        {
            m_idx = i.m_idx;
        }

        IndexMap<int> m;

    }
} // namespace Ra
