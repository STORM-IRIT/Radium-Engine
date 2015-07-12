#include <Core/Index/Index.hpp>

namespace Ra
{

/// CONSTRUCTOR
Index::Index( const int i ) { m_idx = ( i < 0 ) ? s_invalid : i; }
Index::Index( const Index& i ) { m_idx = i.m_idx; }

} // namespace Ra
