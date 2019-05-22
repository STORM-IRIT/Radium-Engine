#include <Core/Asset/HandleData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

HandleComponentData::HandleComponentData() : m_name( "" ), m_frame( Core::Transform::Identity() ) {}

HandleData::HandleData( const std::string& name, const HandleType& type ) :
    AssetData( name ),
    m_frame( Core::Transform::Identity() ),
    m_type( type ),
    m_endNode( false ),
    m_vertexSize( 0 ) {}

HandleData::~HandleData() {}

} // namespace Asset
} // namespace Core
} // namespace Ra
