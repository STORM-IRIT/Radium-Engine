#include <Core/File/GeometryData.hpp>

namespace Ra {
namespace Asset {

GeometryData::GeometryData( const std::string& name, const GeometryType& type ) :
    AssetData( name ),
    m_frame( Core::Transform::Identity() ),
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
    m_loadDuplicates( false ) {}

GeometryData::~GeometryData() {}

} // namespace Asset
} // namespace Ra
