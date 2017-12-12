#ifndef RADIUMENGINE_FULLEDGE_OPERATION_HPP
#define RADIUMENGINE_FULLEDGE_OPERATION_HPP

#include <Core/Index/Index.hpp>
#include <Core/Mesh/DCEL/Definition.hpp>

namespace Ra {
    namespace Core {
        
        void fulledgeSplit( Dcel& dcel, const Index fulledge_id );
        void fulledgeCollapse( Dcel& dcel, const Index fulledge_id );
        
    } // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_FULLEDGE_OPERATION_HPP
