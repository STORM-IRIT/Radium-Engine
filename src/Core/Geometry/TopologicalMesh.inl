#include "TopologicalMesh.hpp"

namespace Ra {
namespace Core {
namespace Geometry {

inline const TopologicalMesh::Normal& TopologicalMesh::normal( VertexHandle vh,
                                                               FaceHandle fh ) const {
    // find halfedge that point to vh and member of fh
    return normal( halfedge_handle( vh, fh ) );
}

inline void TopologicalMesh::set_normal( VertexHandle vh, FaceHandle fh, const Normal& n ) {

    set_normal( halfedge_handle( vh, fh ), n );
}

inline void TopologicalMesh::propagate_normal_to_halfedges( VertexHandle vh ) {
    for ( VertexIHalfedgeIter vih_it = vih_iter( vh ); vih_it.is_valid(); ++vih_it )
    {
        set_normal( *vih_it, normal( vh ) );
    }
}

inline TopologicalMesh::HalfedgeHandle TopologicalMesh::halfedge_handle( VertexHandle vh,
                                                                         FaceHandle fh ) const {
    for ( ConstVertexIHalfedgeIter vih_it = cvih_iter( vh ); vih_it.is_valid(); ++vih_it )
    {
        if ( face_handle( *vih_it ) == fh ) { return *vih_it; }
    }
    CORE_ASSERT( false, "vh is not a vertex of face fh" );
    return HalfedgeHandle();
}

inline const OpenMesh::HPropHandleT<TopologicalMesh::Index>&
TopologicalMesh::getInputTriangleMeshIndexPropHandle() const {
    return m_inputTriangleMeshIndexPph;
}

inline const OpenMesh::HPropHandleT<TopologicalMesh::Index>&
TopologicalMesh::getOutputTriangleMeshIndexPropHandle() const {
    return m_outputTriangleMeshIndexPph;
}

inline const std::vector<OpenMesh::HPropHandleT<float>>&
TopologicalMesh::getFloatPropsHandles() const {
    return m_floatPph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector2>>&
TopologicalMesh::getVector2PropsHandles() const {
    return m_vec2Pph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector3>>&
TopologicalMesh::getVector3PropsHandles() const {
    return m_vec3Pph;
}

inline const std::vector<OpenMesh::HPropHandleT<Vector4>>&
TopologicalMesh::getVector4PropsHandles() const {
    return m_vec4Pph;
}

inline void TopologicalMesh::createNormalPropOnFaces( OpenMesh::FPropHandleT<Normal>& fProp ) {
    auto nph = halfedge_normals_pph();
    add_property( fProp, property( nph ).name() + "_subdiv_copy_F" );
}

inline void TopologicalMesh::clearProp( OpenMesh::FPropHandleT<Normal>& fProp ) {
    remove_property( fProp );
}

inline void TopologicalMesh::copyNormal( HalfedgeHandle input_heh, HalfedgeHandle copy_heh ) {
    auto nph                  = halfedge_normals_pph();
    property( nph, copy_heh ) = property( nph, input_heh );
}

inline void TopologicalMesh::copyNormalFromFace( FaceHandle fh,
                                                 HalfedgeHandle heh,
                                                 OpenMesh::FPropHandleT<Normal> fProp ) {
    auto nph             = halfedge_normals_pph();
    property( nph, heh ) = property( fProp, fh );
}

inline void TopologicalMesh::interpolateNormal( HalfedgeHandle in_a,
                                                HalfedgeHandle in_b,
                                                HalfedgeHandle out,
                                                Scalar f ) {
    auto nph = halfedge_normals_pph();
    property( nph, out ) =
        ( ( 1 - f ) * property( nph, in_a ) + f * property( nph, in_b ) ).normalized();
}

inline void TopologicalMesh::interpolateNormalOnFaces( FaceHandle fh,
                                                       OpenMesh::FPropHandleT<Normal> fProp ) {
    auto nph = halfedge_normals_pph();

    // init sum to first
    auto heh              = halfedge_handle( fh );
    property( fProp, fh ) = property( nph, heh );
    heh                   = next_halfedge_handle( heh );

    // sum others
    for ( size_t i = 1; i < valence( fh ); ++i )
    {
        property( fProp, fh ) += property( nph, heh );
        heh = next_halfedge_handle( heh );
    }

    // normalize
    property( fProp, fh ) = property( fProp, fh ).normalized();
}

template <typename T>
void TopologicalMesh::createPropsOnFaces( const std::vector<OpenMesh::HPropHandleT<T>>& input,
                                          std::vector<OpenMesh::FPropHandleT<T>>& output ) {
    output.reserve( input.size() );
    for ( const auto& oh : input )
    {
        OpenMesh::FPropHandleT<T> oh_;
        add_property( oh_, property( oh ).name() + "_subdiv_copy_F" );
        output.push_back( oh_ );
    }
}

template <typename T>
void TopologicalMesh::clearProps( std::vector<OpenMesh::FPropHandleT<T>>& props ) {
    for ( auto& oh : props )
    {
        remove_property( oh );
    }
    props.clear();
}

template <typename T>
void TopologicalMesh::copyProps( HalfedgeHandle input_heh,
                                 HalfedgeHandle copy_heh,
                                 const std::vector<OpenMesh::HPropHandleT<T>>& props ) {
    for ( const auto& oh : props )
    {
        property( oh, copy_heh ) = property( oh, input_heh );
    }
}

template <typename T>
void TopologicalMesh::copyPropsFromFace( FaceHandle fh,
                                         HalfedgeHandle heh,
                                         const std::vector<OpenMesh::FPropHandleT<T>>& fProps,
                                         const std::vector<OpenMesh::HPropHandleT<T>>& hProps ) {
    for ( uint i = 0; i < fProps.size(); ++i )
    {
        auto hp             = hProps[i];
        auto fp             = fProps[i];
        property( hp, heh ) = property( fp, fh );
    }
}

template <typename T>
void TopologicalMesh::interpolateProps( HalfedgeHandle in_a,
                                        HalfedgeHandle in_b,
                                        HalfedgeHandle out,
                                        Scalar f,
                                        const std::vector<OpenMesh::HPropHandleT<T>>& props ) {
    // interpolate properties
    for ( const auto& oh : props )
    {
        property( oh, out ) = ( 1 - f ) * property( oh, in_a ) + f * property( oh, in_b );
    }
}

template <typename T>
void TopologicalMesh::interpolatePropsOnFaces(
    FaceHandle fh,
    const std::vector<OpenMesh::HPropHandleT<T>>& hProps,
    const std::vector<OpenMesh::FPropHandleT<T>>& fProps ) {
    auto heh       = halfedge_handle( fh );
    const size_t v = valence( fh );

    // init sum to first
    for ( size_t j = 0; j < fProps.size(); ++j )
    {
        auto hp            = hProps[j];
        auto fp            = fProps[j];
        property( fp, fh ) = property( hp, heh );
    }
    heh = next_halfedge_handle( heh );
    // sum others
    for ( size_t i = 1; i < v; ++i )
    {
        for ( size_t j = 0; j < fProps.size(); ++j )
        {
            auto hp = hProps[j];
            auto fp = fProps[j];
            property( fp, fh ) += property( hp, heh );
        }
        heh = next_halfedge_handle( heh );
    }
    // normalize
    for ( size_t j = 0; j < fProps.size(); ++j )
    {
        auto fp            = fProps[j];
        property( fp, fh ) = property( fp, fh ) / v;
    }
}

inline void
TopologicalMesh::createAllPropsOnFaces( OpenMesh::FPropHandleT<Normal>& normalProp,
                                        std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                        std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                        std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                        std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    createNormalPropOnFaces( normalProp );
    createPropsOnFaces( getFloatPropsHandles(), floatProps );
    createPropsOnFaces( getVector2PropsHandles(), vec2Props );
    createPropsOnFaces( getVector3PropsHandles(), vec3Props );
    createPropsOnFaces( getVector4PropsHandles(), vec4Props );
}

inline void
TopologicalMesh::clearAllProps( OpenMesh::FPropHandleT<Normal>& normalProp,
                                std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    clearProp( normalProp );
    clearProps( floatProps );
    clearProps( vec2Props );
    clearProps( vec3Props );
    clearProps( vec4Props );
}

inline void TopologicalMesh::copyAllProps( HalfedgeHandle input_heh, HalfedgeHandle copy_heh ) {
    copyNormal( input_heh, copy_heh );
    copyProps( input_heh, copy_heh, getFloatPropsHandles() );
    copyProps( input_heh, copy_heh, getVector2PropsHandles() );
    copyProps( input_heh, copy_heh, getVector3PropsHandles() );
    copyProps( input_heh, copy_heh, getVector4PropsHandles() );
}

inline void
TopologicalMesh::copyAllPropsFromFace( FaceHandle fh,
                                       HalfedgeHandle heh,
                                       OpenMesh::FPropHandleT<Normal> normalProp,
                                       std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
                                       std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
                                       std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
                                       std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    copyNormalFromFace( fh, heh, normalProp );
    copyPropsFromFace( fh, heh, floatProps, getFloatPropsHandles() );
    copyPropsFromFace( fh, heh, vec2Props, getVector2PropsHandles() );
    copyPropsFromFace( fh, heh, vec3Props, getVector3PropsHandles() );
    copyPropsFromFace( fh, heh, vec4Props, getVector4PropsHandles() );
}

inline void TopologicalMesh::interpolateAllProps( HalfedgeHandle in_a,
                                                  HalfedgeHandle in_b,
                                                  HalfedgeHandle out,
                                                  Scalar f ) {
    interpolateNormal( in_a, in_b, out, f );
    interpolateProps( in_a, in_b, out, f, getFloatPropsHandles() );
    interpolateProps( in_a, in_b, out, f, getVector2PropsHandles() );
    interpolateProps( in_a, in_b, out, f, getVector3PropsHandles() );
    interpolateProps( in_a, in_b, out, f, getVector4PropsHandles() );
}

inline void TopologicalMesh::interpolateAllPropsOnFaces(
    FaceHandle fh,
    OpenMesh::FPropHandleT<Normal> normalProp,
    std::vector<OpenMesh::FPropHandleT<Scalar>>& floatProps,
    std::vector<OpenMesh::FPropHandleT<Vector2>>& vec2Props,
    std::vector<OpenMesh::FPropHandleT<Vector3>>& vec3Props,
    std::vector<OpenMesh::FPropHandleT<Vector4>>& vec4Props ) {
    interpolateNormalOnFaces( fh, normalProp );
    interpolatePropsOnFaces( fh, getFloatPropsHandles(), floatProps );
    interpolatePropsOnFaces( fh, getVector2PropsHandles(), vec2Props );
    interpolatePropsOnFaces( fh, getVector3PropsHandles(), vec3Props );
    interpolatePropsOnFaces( fh, getVector4PropsHandles(), vec4Props );
}

inline std::set<TopologicalMesh::WedgeIndex>
TopologicalMesh::vertex_wedges( OpenMesh::VertexHandle vh ) const {
    std::set<TopologicalMesh::WedgeIndex> ret;

    for ( ConstVertexIHalfedgeIter vh_it = cvih_iter( vh ); vh_it.is_valid(); ++vh_it )
    {
        auto widx = property( m_wedgeIndexPph, *vh_it );
        if ( widx.isValid() && !m_wedges.getWedge( widx ).deleted() ) ret.insert( widx );
    }
    return ret;
}

inline void TopologicalMesh::setWedgeData( TopologicalMesh::WedgeIndex widx,
                                           const TopologicalMesh::WedgeData& wedge ) {
    m_wedges.setWedgeData( widx, wedge );
}

inline const std::vector<std::string>& TopologicalMesh::getVec4AttribNames() const {
    return m_wedges.m_vector4AttribNames;
}
inline const std::vector<std::string>& TopologicalMesh::getVec3AttribNames() const {
    return m_wedges.m_vector3AttribNames;
}
inline const std::vector<std::string>& TopologicalMesh::getVec2AttribNames() const {
    return m_wedges.m_vector2AttribNames;
}
inline const std::vector<std::string>& TopologicalMesh::getFloatAttribNames() const {
    return m_wedges.m_floatAttribNames;
}

inline bool TopologicalMesh::isFeatureVertex( const VertexHandle& vh ) const {
    return vertex_wedges( vh ).size() != 1;
}

inline bool TopologicalMesh::isFeatureEdge( const EdgeHandle& eh ) const {

    auto heh0 = halfedge_handle( eh, 0 );
    auto heh1 = halfedge_handle( eh, 1 );

    return property( m_wedgeIndexPph, heh0 ) ==
               property( m_wedgeIndexPph,
                         prev_halfedge_handle( opposite_halfedge_handle( heh0 ) ) ) ||
           property( m_wedgeIndexPph, heh1 ) ==
               property( m_wedgeIndexPph,
                         prev_halfedge_handle( opposite_halfedge_handle( heh1 ) ) );
}

inline const OpenMesh::HPropHandleT<TopologicalMesh::WedgeIndex>&
TopologicalMesh::getWedgeIndexPph() const {
    return m_wedgeIndexPph;
}

/////////////// WEDGES RELATED STUFF /////////////////
inline void TopologicalMesh::WedgeCollection::del( const TopologicalMesh::WedgeIndex& idx ) {
    if ( idx.isValid() ) m_data[idx].decrementRefCount();
}

inline const TopologicalMesh::Wedge&
TopologicalMesh::WedgeCollection::getWedge( const TopologicalMesh::WedgeIndex& idx ) const {
    return m_data[idx];
}

inline void TopologicalMesh::WedgeCollection::setWedgeData( const TopologicalMesh::WedgeIndex& idx,
                                                            const TopologicalMesh::WedgeData& wd ) {
    if ( !( wd.m_floatAttrib.size() == m_floatAttribNames.size() &&
            wd.m_vector2Attrib.size() == m_vector2AttribNames.size() &&
            wd.m_vector3Attrib.size() == m_vector3AttribNames.size() &&
            wd.m_vector4Attrib.size() == m_vector4AttribNames.size() ) )
    {
        LOG( logWARNING ) << "Warning, topological mesh set wedge: number of attribs inconsistency";
    }
    if ( idx.isValid() ) m_data[idx].setWedgeData( wd );
}

template <typename T>
void TopologicalMesh::WedgeCollection::addProp( std::string name ) {
    if ( name != std::string( "in_position" ) )
    {
        if ( std::is_same<T, float>::value )
            m_floatAttribNames.push_back( name );
        else if ( std::is_same<T, Vector2>::value )
            m_vector2AttribNames.push_back( name );
        else if ( std::is_same<T, Vector3>::value )
            m_vector3AttribNames.push_back( name );
        else if ( std::is_same<T, Vector4>::value )
            m_vector4AttribNames.push_back( name );
        else
            LOG( logWARNING )
                << "Warning, mesh attribute " << name
                << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
    }
}

inline void TopologicalMesh::WedgeCollection::garbageCollection() {
    std::remove_if( m_data.begin(), m_data.end(), []( const Wedge& w ) { return w.deleted(); } );
}

inline bool TopologicalMesh::WedgeData::operator==( const TopologicalMesh::WedgeData& lhs ) const {
    return
        //     m_inputTriangleMeshIndex == lhs.m_inputTriangleMeshIndex &&
        // m_outputTriangleMeshIndex == lhs.m_outputTriangleMeshIndex &&
        m_position == lhs.m_position && m_floatAttrib == lhs.m_floatAttrib &&
        m_vector2Attrib == lhs.m_vector2Attrib && m_vector3Attrib == lhs.m_vector3Attrib &&
        m_vector4Attrib == lhs.m_vector4Attrib;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
