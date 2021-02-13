#pragma once

#include "TopologicalMesh.hpp"

#include <typeinfo>
#include <unordered_map>

namespace Ra {
namespace Core {
namespace Geometry {

template <typename NonManifoldFaceCommand>
inline TopologicalMesh::TopologicalMesh( const TriangleMesh& triMesh,
                                         NonManifoldFaceCommand command ) {

    LOG( logINFO ) << "TopologicalMesh: load triMesh with " << triMesh.getIndices().size()
                   << " faces and " << triMesh.vertices().size() << " vertices.";

    struct hash_vec {
        size_t operator()( const Vector3& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue[0] );
            size_t hy = std::hash<Scalar>()( lvalue[1] );
            size_t hz = std::hash<Scalar>()( lvalue[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };
    // use a hashmap for fast search of existing vertex position
    using VertexMap = std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec>;
    VertexMap vertexHandles;

    add_property( m_inputTriangleMeshIndexPph );
    add_property( m_wedgeIndexPph );

    std::vector<PropPair<float>> vprop_float;
    std::vector<std::pair<AttribHandle<Vector2>, OpenMesh::HPropHandleT<Vector2>>> vprop_vec2;
    std::vector<std::pair<AttribHandle<Vector3>, OpenMesh::HPropHandleT<Vector3>>> vprop_vec3;
    std::vector<std::pair<AttribHandle<Vector4>, OpenMesh::HPropHandleT<Vector4>>> vprop_vec4;

    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib(
        [&triMesh, this, &vprop_float, &vprop_vec2, &vprop_vec3, &vprop_vec4]( const auto& attr ) {
            // skip builtin attribs
            if ( attr->getName() != std::string( "in_position" ) &&
                 attr->getName() != std::string( "in_normal" ) )
            {
                if ( attr->isFloat() )
                    addAttribPairToTopo( triMesh, attr, vprop_float, m_floatPph );
                else if ( attr->isVector2() )
                    addAttribPairToTopo( triMesh, attr, vprop_vec2, m_vec2Pph );
                else if ( attr->isVector3() )
                    addAttribPairToTopo( triMesh, attr, vprop_vec3, m_vec3Pph );
                else if ( attr->isVector4() )
                    addAttribPairToTopo( triMesh, attr, vprop_vec4, m_vec4Pph );
                else
                    LOG( logWARNING )
                        << "Warning, mesh attribute " << attr->getName()
                        << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
            }
        } );

    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib( InitWedgeProps {this, triMesh} );

    size_t num_triangles = triMesh.getIndices().size();

    command.initialize( triMesh );

    const bool hasNormals = !triMesh.normals().empty();
    if ( !hasNormals )
    {
        release_face_normals();
        release_vertex_normals();
        release_halfedge_normals();
    }
    for ( unsigned int i = 0; i < num_triangles; i++ )
    {
        std::vector<TopologicalMesh::VertexHandle> face_vhandles( 3 );
        std::vector<TopologicalMesh::Normal> face_normals( 3 );
        std::vector<unsigned int> face_vertexIndex( 3 );
        std::vector<WedgeIndex> face_wedges( 3 );
        const auto& triangle = triMesh.getIndices()[i];
        for ( size_t j = 0; j < 3; ++j )
        {
            unsigned int inMeshVertexIndex = triangle[j];
            const Vector3& p               = triMesh.vertices()[inMeshVertexIndex];

            typename VertexMap::iterator vtr = vertexHandles.find( p );

            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, typename VertexMap::value_type( p, vh ) );
            }
            else
            { vh = vtr->second; }

            face_vhandles[j]    = vh;
            face_vertexIndex[j] = inMeshVertexIndex;
            if ( hasNormals ) face_normals[j] = triMesh.normals()[inMeshVertexIndex];

            WedgeData wd;
            wd.m_position = p;

            copyMeshToWedgeData( triMesh,
                                 inMeshVertexIndex,
                                 m_wedges.m_wedgeFloatAttribHandles,
                                 m_wedges.m_wedgeVector2AttribHandles,
                                 m_wedges.m_wedgeVector3AttribHandles,
                                 m_wedges.m_wedgeVector4AttribHandles,
                                 &wd );

            face_wedges[j] = m_wedges.add( wd );
        }

        // Add the face, then add attribs to vh
        auto fh = add_face( face_vhandles );
        // In case of topological inconsistancy, face will be invalid ...
        if ( fh.is_valid() )
        {
            for ( size_t vindex = 0; vindex < face_vhandles.size(); vindex++ )
            {
                TopologicalMesh::HalfedgeHandle heh = halfedge_handle( face_vhandles[vindex], fh );
                if ( hasNormals ) set_normal( heh, face_normals[vindex] );
                property( m_inputTriangleMeshIndexPph, heh ) = face_vertexIndex[vindex];
                copyAttribToTopo( triMesh, vprop_float, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, vprop_vec2, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, vprop_vec3, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, vprop_vec4, heh, face_vertexIndex[vindex] );
                property( m_wedgeIndexPph, heh ) = face_wedges[vindex];
            }
        }
        else
        {
            for ( auto wedgeIndex : face_wedges )
                m_wedges.del( wedgeIndex );
            command.process( face_vhandles );
        }
        face_vhandles.clear();
        face_normals.clear();
        face_vertexIndex.clear();
    }
    command.postProcess( *this );
}

template <typename NonManifoldFaceCommand>
void TopologicalMesh::initWithWedge( const TriangleMesh& triMesh, NonManifoldFaceCommand command ) {

    LOG( logINFO ) << "TopologicalMesh: load triMesh with " << triMesh.getIndices().size()
                   << " faces and " << triMesh.vertices().size() << " vertices.";

    ///\todo use a kdtree
    struct hash_vec {
        size_t operator()( const Vector3& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue[0] );
            size_t hy = std::hash<Scalar>()( lvalue[1] );
            size_t hz = std::hash<Scalar>()( lvalue[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };
    // use a hashmap for fast search of existing vertex position
    using VertexMap = std::unordered_map<Vector3, TopologicalMesh::VertexHandle, hash_vec>;
    VertexMap vertexHandles;

    add_property( m_inputTriangleMeshIndexPph );
    add_property( m_wedgeIndexPph );

    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib( InitWedgeProps {this, triMesh} );

    size_t num_triangles = triMesh.getIndices().size();

    for ( size_t i = 0; i < triMesh.vertices().size(); ++i )
    {
        // create an empty wedge, with 0 ref
        Wedge w;

        WedgeData wd;
        wd.m_position = triMesh.vertices()[i];
        copyMeshToWedgeData( triMesh,
                             i,
                             m_wedges.m_wedgeFloatAttribHandles,
                             m_wedges.m_wedgeVector2AttribHandles,
                             m_wedges.m_wedgeVector3AttribHandles,
                             m_wedges.m_wedgeVector4AttribHandles,
                             &wd );
        // here ref is not incremented
        w.setWedgeData( std::move( wd ) );
        // the newly added wedge is not referenced yet, will be done with `newReference` when
        // creating faces just below
        m_wedges.m_data.push_back( w );
    }

    LOG( logINFO ) << "TopologicalMesh: have  " << m_wedges.size() << " wedges ";

    const bool hasNormals = !triMesh.normals().empty();
    if ( !hasNormals )
    {
        release_face_normals();
        release_vertex_normals();
        release_halfedge_normals();
    }

    command.initialize( triMesh );
    for ( unsigned int i = 0; i < num_triangles; i++ )
    {
        std::vector<TopologicalMesh::VertexHandle> face_vhandles( 3 );
        std::vector<TopologicalMesh::Normal> face_normals( 3 );
        std::vector<WedgeIndex> face_wedges( 3 );
        const auto& triangle = triMesh.getIndices()[i];

        for ( size_t j = 0; j < 3; ++j )
        {
            unsigned int inMeshVertexIndex = triangle[j];
            const Vector3& p               = triMesh.vertices()[inMeshVertexIndex];

            typename VertexMap::iterator vtr = vertexHandles.find( p );
            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, typename VertexMap::value_type( p, vh ) );
            }
            else
            { vh = vtr->second; }

            face_vhandles[j] = vh;
            if ( hasNormals ) face_normals[j] = triMesh.normals()[inMeshVertexIndex];
            face_wedges[j] =
                WedgeIndex {static_cast<WedgeIndex::Index::IntegerType>( inMeshVertexIndex )};
        }

        // Add the face, then add attribs to vh
        TopologicalMesh::FaceHandle fh = add_face( face_vhandles );
        if ( fh.is_valid() )
        {
            for ( size_t vindex = 0; vindex < face_vhandles.size(); vindex++ )
            {
                TopologicalMesh::HalfedgeHandle heh = halfedge_handle( face_vhandles[vindex], fh );
                if ( hasNormals ) set_normal( heh, face_normals[vindex] );
                property( m_wedgeIndexPph, heh ) = m_wedges.newReference( face_wedges[vindex] );
            }
        }
        else
        { command.process( face_vhandles ); }
        face_vhandles.clear();
        face_normals.clear();
    }
    command.postProcess( *this );

    LOG( logINFO ) << "TopologicalMesh: load end with  " << m_wedges.size() << " wedges ";
}

template <typename T>
void TopologicalMesh::copyAttribToWedgeData( const TriangleMesh& mesh,
                                             unsigned int vindex,
                                             const std::vector<AttribHandle<T>>& attrHandleVec,
                                             VectorArray<T>* to ) {
    for ( auto handle : attrHandleVec )
    {
        auto& attr = mesh.getAttrib<T>( handle );
        to->push_back( attr.data()[vindex] );
    }
}

template <typename T>
void TopologicalMesh::addAttribPairToTopo( const TriangleMesh& triMesh,
                                           AttribManager::pointer_type attr,
                                           std::vector<TopologicalMesh::PropPair<T>>& vprop,
                                           std::vector<OpenMesh::HPropHandleT<T>>& pph ) {
    AttribHandle<T> h = triMesh.getAttribHandle<T>( attr->getName() );
    if ( attr->getSize() == triMesh.vertices().size() )
    {
        OpenMesh::HPropHandleT<T> oh;
        this->add_property( oh, attr->getName() );
        vprop.push_back( std::make_pair( h, oh ) );
        pph.push_back( oh );
    }
    else
    {
        LOG( logWARNING ) << "[TopologicalMesh] Skip badly sized attribute " << attr->getName()
                          << ".";
    }
}

void TopologicalMesh::copyMeshToWedgeData( const TriangleMesh& mesh,
                                           unsigned int vindex,
                                           const std::vector<AttribHandle<float>>& wprop_float,
                                           const std::vector<AttribHandle<Vector2>>& wprop_vec2,
                                           const std::vector<AttribHandle<Vector3>>& wprop_vec3,
                                           const std::vector<AttribHandle<Vector4>>& wprop_vec4,
                                           TopologicalMesh::WedgeData* wd ) {

    copyAttribToWedgeData( mesh, vindex, wprop_float, &wd->m_floatAttrib );
    copyAttribToWedgeData( mesh, vindex, wprop_vec2, &wd->m_vector2Attrib );
    copyAttribToWedgeData( mesh, vindex, wprop_vec3, &wd->m_vector3Attrib );
    copyAttribToWedgeData( mesh, vindex, wprop_vec4, &wd->m_vector4Attrib );
}

template <typename T>
void TopologicalMesh::copyAttribToTopo( const TriangleMesh& triMesh,
                                        const std::vector<PropPair<T>>& vprop,
                                        TopologicalMesh::HalfedgeHandle heh,
                                        unsigned int vindex ) {
    for ( auto pp : vprop )
    {
        this->property( pp.second, heh ) = triMesh.getAttrib( pp.first ).data()[vindex];
    }
}

inline const TopologicalMesh::Normal& TopologicalMesh::normal( VertexHandle vh,
                                                               FaceHandle fh ) const {
    // find halfedge that point to vh and member of fh
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, return dummy ref to  (0,0,0)";
        static TopologicalMesh::Normal dummy {0_ra, 0_ra, 0_ra};
        return dummy;
    }
    return normal( halfedge_handle( vh, fh ) );
}

inline void TopologicalMesh::set_normal( VertexHandle vh, FaceHandle fh, const Normal& n ) {
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }

    set_normal( halfedge_handle( vh, fh ), n );
}

inline void TopologicalMesh::propagate_normal_to_halfedges( VertexHandle vh ) {
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }
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
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }
    auto nph = halfedge_normals_pph();
    add_property( fProp, property( nph ).name() + "_subdiv_copy_F" );
}

inline void TopologicalMesh::clearProp( OpenMesh::FPropHandleT<Normal>& fProp ) {
    remove_property( fProp );
}

inline void TopologicalMesh::copyNormal( HalfedgeHandle input_heh, HalfedgeHandle copy_heh ) {
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }
    auto nph                  = halfedge_normals_pph();
    property( nph, copy_heh ) = property( nph, input_heh );
}

inline void TopologicalMesh::copyNormalFromFace( FaceHandle fh,
                                                 HalfedgeHandle heh,
                                                 OpenMesh::FPropHandleT<Normal> fProp ) {
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }
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
    if ( !has_halfedge_normals() )
    {
        LOG( logERROR ) << "TopologicalMesh has no normals, nothing set";
        return;
    }
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
TopologicalMesh::getVertexWedges( OpenMesh::VertexHandle vh ) const {
    std::set<TopologicalMesh::WedgeIndex> ret;

    for ( ConstVertexIHalfedgeIter vh_it = cvih_iter( vh ); vh_it.is_valid(); ++vh_it )
    {
        auto widx = property( m_wedgeIndexPph, *vh_it );
        if ( widx.isValid() && !m_wedges.getWedge( widx ).isDeleted() ) ret.insert( widx );
    }
    return ret;
}

inline const TopologicalMesh::WedgeData&
TopologicalMesh::getWedgeData( const WedgeIndex& idx ) const {
    return m_wedges.getWedgeData( idx );
}

inline unsigned int TopologicalMesh::getWedgeRefCount( const WedgeIndex& idx ) const {
    return m_wedges.getWedgeRefCount( idx );
}

inline void TopologicalMesh::setWedgeData( TopologicalMesh::WedgeIndex widx,
                                           const TopologicalMesh::WedgeData& wedge ) {
    m_wedges.setWedgeData( widx, wedge );
}

template <typename T>
inline bool TopologicalMesh::setWedgeData( const TopologicalMesh::WedgeIndex& idx,
                                           const std::string& name,
                                           const T& value ) {
    return m_wedges.setWedgeData( idx, name, value );
}

inline void TopologicalMesh::replaceWedge( OpenMesh::HalfedgeHandle he, const WedgeData& wd ) {
    m_wedges.del( property( getWedgeIndexPph(), he ) );
    property( getWedgeIndexPph(), he ) = m_wedges.add( wd );
}

inline void TopologicalMesh::replaceWedgeIndex( OpenMesh::HalfedgeHandle he,
                                                const WedgeIndex& widx ) {
    m_wedges.del( property( getWedgeIndexPph(), he ) );
    property( getWedgeIndexPph(), he ) = m_wedges.newReference( widx );
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
    return getVertexWedges( vh ).size() != 1;
}

inline bool TopologicalMesh::isFeatureEdge( const EdgeHandle& eh ) const {

    auto heh0 = halfedge_handle( eh, 0 );
    auto heh1 = halfedge_handle( eh, 1 );

    return property( m_wedgeIndexPph, heh0 ) !=
               property( m_wedgeIndexPph,
                         prev_halfedge_handle( opposite_halfedge_handle( heh0 ) ) ) ||
           property( m_wedgeIndexPph, heh1 ) !=
               property( m_wedgeIndexPph,
                         prev_halfedge_handle( opposite_halfedge_handle( heh1 ) ) );
}

inline const OpenMesh::HPropHandleT<TopologicalMesh::WedgeIndex>&
TopologicalMesh::getWedgeIndexPph() const {
    return m_wedgeIndexPph;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////      WEDGES RELATED STUFF     //////////////////////////////
///////////////////      WedgeCollection          //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

inline void TopologicalMesh::WedgeCollection::del( const TopologicalMesh::WedgeIndex& idx ) {
    if ( idx.isValid() ) m_data[idx].decrementRefCount();
}

inline TopologicalMesh::WedgeIndex
TopologicalMesh::WedgeCollection::newReference( const TopologicalMesh::WedgeIndex& idx ) {
    if ( idx.isValid() ) m_data[idx].incrementRefCount();
    return idx;
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

#define GET_NAME_ARRAY_HELPER( TYPE, NAME )                                                       \
    template <>                                                                                   \
    inline const std::vector<std::string>& TopologicalMesh::WedgeCollection::getNameArray<TYPE>() \
        const {                                                                                   \
        return m_##NAME##AttribNames;                                                             \
    }                                                                                             \
    template <>                                                                                   \
    inline std::vector<std::string>& TopologicalMesh::WedgeCollection::getNameArray<TYPE>() {     \
        return m_##NAME##AttribNames;                                                             \
    }

GET_NAME_ARRAY_HELPER( float, float )
GET_NAME_ARRAY_HELPER( Vector2, vector2 )
GET_NAME_ARRAY_HELPER( Vector3, vector3 )
GET_NAME_ARRAY_HELPER( Vector4, vector4 )

#undef GET_NAME_ARRAY_HELPER
// These template functions are defined above for supported types.
// For unsupported types they simply generate a compile error.
template <typename T>
inline const std::vector<std::string>& TopologicalMesh::WedgeCollection::getNameArray() const {

    LOG( logWARNING ) << "Warning, mesh attribute " << typeid( T ).name()
                      << " is not supported (only float, vec2, vec3 nor vec4 are supported)";
    static_assert( sizeof( T ) == -1, "this type is not supported" );
    return m_floatAttribNames;
}

template <typename T>
inline std::vector<std::string>& TopologicalMesh::WedgeCollection::getNameArray() {

    LOG( logWARNING ) << "Warning, mesh attribute " << typeid( T ).name()
                      << " is not supported (only float, vec2, vec3 nor vec4 are supported)";
    static_assert( sizeof( T ) == -1, "this type is not supported" );
    return m_floatAttribNames;
}

template <typename T>
inline bool TopologicalMesh::WedgeCollection::setWedgeData( const TopologicalMesh::WedgeIndex& idx,
                                                            const std::string& name,
                                                            const T& value ) {
    if ( idx.isValid() )
    {
        auto nameArray = getNameArray<T>();
        auto itr       = std::find( nameArray.begin(), nameArray.end(), name );
        if ( itr != nameArray.end() )
        {
            auto attrIndex = std::distance( nameArray.begin(), itr );
            m_data[idx].getWedgeData().getAttribArray<T>()[attrIndex] = value;
            return true;
        }
        else
        {
            LOG( logERROR ) << "Warning, set wedge: no wedge attrib named " << name << " of type "
                            << typeid( T ).name();
        }
    }
    return false;
}

inline bool
TopologicalMesh::WedgeCollection::setWedgePosition( const TopologicalMesh::WedgeIndex& idx,
                                                    const Vector3& value ) {
    if ( idx.isValid() )
    {
        m_data[idx].getWedgeData().m_position = value;
        return true;
    }
    return false;
}

template <typename T>
void TopologicalMesh::WedgeCollection::addProp( const std::string& name ) {
    if ( name != std::string( "in_position" ) ) { getNameArray<T>().push_back( name ); }
}

inline void TopologicalMesh::WedgeCollection::garbageCollection() {
    m_data.erase( std::remove_if( m_data.begin(),
                                  m_data.end(),
                                  []( const Wedge& w ) { return w.isDeleted(); } ),
                  m_data.end() );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////      WedgeData                //////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// return 1 : equals, 2: strict less, 3: strict greater
template <typename T>
int TopologicalMesh::WedgeData::compareVector( const T& a, const T& b ) {
    for ( int i = 0; i < T::RowsAtCompileTime; i++ )
    {
        if ( a[i] < b[i] ) return 2;
        if ( a[i] > b[i] ) return 3;
    }
    // (a == b)
    return 1;
}

inline bool TopologicalMesh::WedgeData::operator==( const TopologicalMesh::WedgeData& lhs ) const {
    return
        // do not have this yet, not sure we need to test them
        // m_inputTriangleMeshIndex == lhs.m_inputTriangleMeshIndex &&
        // m_outputTriangleMeshIndex == lhs.m_outputTriangleMeshIndex &&
        m_position == lhs.m_position && m_floatAttrib == lhs.m_floatAttrib &&
        m_vector2Attrib == lhs.m_vector2Attrib && m_vector3Attrib == lhs.m_vector3Attrib &&
        m_vector4Attrib == lhs.m_vector4Attrib;
}

inline bool TopologicalMesh::WedgeData::operator<( const TopologicalMesh::WedgeData& lhs ) const {

    CORE_ASSERT( ( m_floatAttrib.size() == lhs.m_floatAttrib.size() ) &&
                     ( m_vector2Attrib.size() == lhs.m_vector2Attrib.size() ) &&
                     ( m_vector3Attrib.size() == lhs.m_vector3Attrib.size() ) &&
                     ( m_vector4Attrib.size() == lhs.m_vector4Attrib.size() ),
                 "Could only compare wedge with same number of attributes" );

    {
        int comp = compareVector( m_position, lhs.m_position );
        if ( comp == 2 ) return true;
        if ( comp == 3 ) return false;
    }
    for ( size_t i = 0; i < m_floatAttrib.size(); i++ )
    {
        if ( m_floatAttrib[i] < lhs.m_floatAttrib[i] )
            return true;
        else if ( m_floatAttrib[i] > lhs.m_floatAttrib[i] )
            return false;
    }

    for ( size_t i = 0; i < m_vector2Attrib.size(); i++ )
    {
        int comp = compareVector( m_vector2Attrib[i], lhs.m_vector2Attrib[i] );
        if ( comp == 2 ) return true;
        if ( comp == 3 ) return false;
    }
    for ( size_t i = 0; i < m_vector3Attrib.size(); i++ )
    {
        int comp = compareVector( m_vector3Attrib[i], lhs.m_vector3Attrib[i] );
        if ( comp == 2 ) return true;
        if ( comp == 3 ) return false;
    }
    for ( size_t i = 0; i < m_vector4Attrib.size(); i++ )
    {
        int comp = compareVector( m_vector4Attrib[i], lhs.m_vector4Attrib[i] );
        if ( comp == 2 ) return true;
        if ( comp == 3 ) return false;
    }
    return false;
}

bool TopologicalMesh::WedgeData::operator!=( const TopologicalMesh::WedgeData& lhs ) const {
    return !( *this == lhs );
}

#define GET_ATTRIB_ARRAY_HELPER( TYPE, NAME )                                      \
    template <>                                                                    \
    inline VectorArray<TYPE>& TopologicalMesh::WedgeData::getAttribArray<TYPE>() { \
        return m_##NAME##Attrib;                                                   \
    }

GET_ATTRIB_ARRAY_HELPER( float, float )
GET_ATTRIB_ARRAY_HELPER( Vector2, vector2 )
GET_ATTRIB_ARRAY_HELPER( Vector3, vector3 )
GET_ATTRIB_ARRAY_HELPER( Vector4, vector4 )
#undef GET_ATTRIB_ARRAY_HELPER

template <typename T>
inline VectorArray<T>& TopologicalMesh::WedgeData::getAttribArray() {
    static_assert( sizeof( T ) == -1, "this type is not supported" );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
