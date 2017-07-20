#include <Core/File/GeometryData.hpp>

namespace Ra {
namespace Asset {

/// CONSTRUCTOR
MaterialData::MaterialData()
    : m_diffuse()
    , m_specular()
    , m_shininess()
    , m_opacity(1.0)
    , m_texDiffuse("")
    , m_texSpecular("")
    , m_texShininess("")
    , m_texNormal("")
    , m_texOpacity("")
    , m_hasDiffuse( false )
    , m_hasSpecular( false )
    , m_hasShininess( false )
    , m_hasOpacity(false)
    , m_hasTexDiffuse(false)
    , m_hasTexSpecular(false)
    , m_hasTexShininess(false)
    , m_hasTexNormal(false)
    , m_hasTexOpacity(false)
{ }


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
    m_hasMaterial( false ),
    m_loadDuplicates( false ) { }

/// DESTRUCTOR
GeometryData::~GeometryData() { }


} // namespace Asset
} // namespace Ra
