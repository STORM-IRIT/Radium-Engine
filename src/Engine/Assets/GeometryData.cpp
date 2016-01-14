#include <Engine/Assets/GeometryData.hpp>

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
MaterialData::MaterialData() :
    m_diffuse(),
    m_specular(),
    m_shininess(),
    m_texDiffuse(""),
    m_texSpecular(""),
    m_texShininess(""),
    m_texNormal(""),
    m_texOpacity(""),
    m_hasDiffuse( false ),
    m_hasSpecular( false ),
    m_hasShininess( false ) { }


/// CONSTRUCTOR
GeometryData::GeometryData( const std::string&  name,
                            const GeometryType& type ) :
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
    m_hasMaterial( false ) { }

/// DESTRUCTOR
GeometryData::~GeometryData() { }


} // namespace Asset
} // namespace Ra
