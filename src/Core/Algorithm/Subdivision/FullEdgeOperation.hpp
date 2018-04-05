#ifndef RADIUMENGINE_FULLEDGE_OPERATION_HPP
#define RADIUMENGINE_FULLEDGE_OPERATION_HPP

#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

class Dcel;
class Index;

[[deprecated("Dcel will be removed without notice.")]]
RA_CORE_API void fulledgeSplit( Dcel& dcel, const Index fulledge_id );
[[deprecated("Dcel will be removed without notice.")]]
RA_CORE_API void fulledgeCollapse( Dcel& dcel, const Index fulledge_id );

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_FULLEDGE_OPERATION_HPP
