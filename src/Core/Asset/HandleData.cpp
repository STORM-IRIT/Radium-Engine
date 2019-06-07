#include <Core/Asset/HandleData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

HandleComponentData::HandleComponentData() :
    m_name( "" ),
    m_frame( Core::Transform::Identity() ),
    m_weight() {}

HandleData::HandleData( const std::string& name, const HandleType& type ) :
    AssetData( name ),
    m_frame( Core::Transform::Identity() ),
    m_type( type ),
    m_endNode( false ),
    m_vertexSize( 0 ),
    m_nameTable(),
    m_component(),
    m_edge(),
    m_face() {}

HandleData::~HandleData() {}

} // namespace Asset
} // namespace Core
} // namespace Ra
