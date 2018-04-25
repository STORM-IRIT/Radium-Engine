#include <Core/Asset/GeometryData.hpp>

namespace Ra {
namespace Core {
namespace Asset {

/// CONSTRUCTOR
GeometryData::GeometryData( const std::string& name, const GeometryType& type ) :
    AssetData( name ),
    m_frame( Core::Math::Transform::Identity() ),
    m_type( type ),
    m_vertex(),
    m_edge(),
    m_faces(),
    m_polyhedron(),
    m_normal(),
    m_tangent(),
    m_bitangent(),
    m_texCoord(),
    m_color(),
    m_material(),
    m_hasMaterial( false ),
    m_loadDuplicates( false ) {}

/// DESTRUCTOR
GeometryData::~GeometryData() {}

} // namespace Asset
} // namespace Core
} // namespace Ra
