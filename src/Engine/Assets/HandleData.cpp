#include <Engine/Assets/HandleData.hpp>

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
HandleComponentData::HandleComponentData() :
    m_name( "" ),
    m_weight(),
    m_frame( Core::Transform::Identity() ) { }



/// CONSTRUCTOR
HandleData::HandleData( const std::string& name,
                        const HandleType&  type ) :
    AssetData( name ),
    m_type( type ),
    m_frame( Core::Transform::Identity() ),
    m_endNode( false ),
    m_vertexSize( 0 ),
    m_nameTable(),
    m_component(),
    m_edge(),
    m_face() { }

/// DESTRUCTOR
HandleData::~HandleData() { }


} // namespace Asset
} // namespace Ra
