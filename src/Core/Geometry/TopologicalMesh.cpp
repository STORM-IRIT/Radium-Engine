#include <Core/Geometry/TopologicalMesh.hpp>

#include <Core/RaCore.hpp>
#include <Core/Utils/Log.hpp>

#include <Eigen/StdVector>

#include <unordered_map>
#include <utility>
#include <vector>

namespace Ra {
namespace Core {
namespace Geometry {

using namespace Utils; // log, AttribXXX

template <typename T>
using PropPair = std::pair<AttribHandle<T>, OpenMesh::HPropHandleT<T>>;

///////////////// HELPERS ///////////////

std::string wedgeInfo( const Ra::Core::Geometry::TopologicalMesh& topo,
                       const Ra::Core::Geometry::TopologicalMesh::WedgeIndex& idx ) {

    std::stringstream buffer;
    if ( !idx.isValid() )
    {
        buffer << "wedge (invalid) ";
        return buffer.str();
    }

    const Ra::Core::Geometry::TopologicalMesh::WedgeData& wd = topo.getWedgeData( idx );

    buffer << "wedge (" << idx << "," << topo.getWedgeRefCount( idx ) << "), ";
    auto& floatAttrNames = topo.getFloatAttribNames();
    for ( size_t i = 0; i < floatAttrNames.size(); ++i )
    {
        buffer << floatAttrNames[i];
        buffer << "[";
        buffer << wd.m_floatAttrib[i];
        buffer << "], ";
    }
    auto vec2AttrNames = topo.getVec2AttribNames();
    for ( size_t i = 0; i < vec2AttrNames.size(); ++i )
    {
        buffer << vec2AttrNames[i];
        buffer << "[";
        buffer << wd.m_vector2Attrib[i].transpose();
        buffer << "], ";
    }
    auto vec3AttrNames = topo.getVec3AttribNames();
    for ( size_t i = 0; i < vec3AttrNames.size(); ++i )
    {
        buffer << vec3AttrNames[i];
        buffer << "[";
        buffer << wd.m_vector3Attrib[i].transpose();
        buffer << "], ";
    }

    auto vec4AttrNames = topo.getVec4AttribNames();
    for ( size_t i = 0; i < vec4AttrNames.size(); ++i )
    {
        buffer << vec4AttrNames[i];
        buffer << "[";
        buffer << wd.m_vector4Attrib[i].transpose();
        buffer << "], ";
    }

    return buffer.str();
}

bool TopologicalMesh::checkIntegrity() const {
    std::vector<unsigned int> count( m_wedges.size(), 0 );
    bool ret = true;
    for ( auto he_itr {halfedges_begin()}; he_itr != halfedges_end(); ++he_itr )
    {
        auto widx = property( m_wedgeIndexPph, *he_itr );
        if ( widx.isValid() )
        {
            count[widx]++;

            if ( m_wedges.getWedgeData( widx ).m_position != point( to_vertex_handle( *he_itr ) ) )
            {
                LOG( logWARNING ) << "topological mesh wedge inconsistency, wedge and to position "
                                     "differ for widx "
                                  << widx << ", have "
                                  << m_wedges.getWedgeData( widx ).m_position.transpose()
                                  << "instead of "
                                  << point( to_vertex_handle( *he_itr ) ).transpose();
            }
        }
    }

    for ( int widx = 0; widx < int( m_wedges.size() ); ++widx )
    {
        if ( m_wedges.getWedge( WedgeIndex {widx} ).getRefCount() != count[widx] )
        {
            LOG( logWARNING ) << "topological mesh wedge count inconsistency, have  " << count[widx]
                              << " instead of "
                              << m_wedges.getWedge( WedgeIndex {widx} ).getRefCount()
                              << " for wedge id " << widx;
            ret = false;
        }
    }
    return ret;
}

void printWedgesInfo( const Ra::Core::Geometry::TopologicalMesh& topo ) {
    using namespace Ra::Core;

    for ( auto itr = topo.vertices_sbegin(); itr != topo.vertices_end(); ++itr )
    {
        LOG( Utils::logINFO ) << "vertex " << *itr;
        auto wedges = topo.getVertexWedges( *itr );
        for ( auto wedgeIndex : wedges )
        {
            LOG( Utils::logINFO ) << wedgeInfo( topo, wedgeIndex );
        }
    }

    for ( auto itr = topo.halfedges_sbegin(); itr != topo.halfedges_end(); ++itr )
    {
        LOG( Utils::logINFO ) << "he " << *itr
                              << ( topo.is_boundary( *itr ) ? " boundary " : " inner " );
        LOG( Utils::logINFO ) << wedgeInfo( topo, topo.property( topo.getWedgeIndexPph(), *itr ) );
    }
}

template <typename T>
void addAttribPairToTopo( const TriangleMesh& triMesh,
                          TopologicalMesh* topoMesh,
                          AttribManager::pointer_type attr,
                          std::vector<PropPair<T>>& vprop,
                          std::vector<OpenMesh::HPropHandleT<T>>& pph ) {
    AttribHandle<T> h = triMesh.getAttribHandle<T>( attr->getName() );
    if ( attr->getSize() == triMesh.vertices().size() )
    {
        OpenMesh::HPropHandleT<T> oh;
        topoMesh->add_property( oh, attr->getName() );
        vprop.push_back( std::make_pair( h, oh ) );
        pph.push_back( oh );
    }
    else
    {
        LOG( logWARNING ) << "[TopologicalMesh] Skip badly sized attribute " << attr->getName()
                          << ".";
    }
}

template <typename T>
void addAttribPairToCore( TriangleMesh& triMesh,
                          const TopologicalMesh* topoMesh,
                          OpenMesh::HPropHandleT<T> oh,
                          std::vector<PropPair<T>>& vprop ) {
    AttribHandle<T> h = triMesh.addAttrib<T>( topoMesh->property( oh ).name() );
    vprop.push_back( std::make_pair( h, oh ) );
}

template <typename T>
void copyAttribToTopo( const TriangleMesh& triMesh,
                       TopologicalMesh* topoMesh,
                       const std::vector<PropPair<T>>& vprop,
                       TopologicalMesh::HalfedgeHandle heh,
                       unsigned int vindex ) {
    for ( auto pp : vprop )
    {
        topoMesh->property( pp.second, heh ) = triMesh.getAttrib( pp.first ).data()[vindex];
    }
}

template <typename T>
using HandleAndValueVector = std::vector<std::pair<AttribHandle<T>, T>,
                                         Eigen::aligned_allocator<std::pair<AttribHandle<T>, T>>>;

template <typename T>
void copyAttribToCoreVertex( HandleAndValueVector<T>& data,
                             const TopologicalMesh* topoMesh,
                             const std::vector<PropPair<T>>& vprop,
                             TopologicalMesh::HalfedgeHandle heh ) {
    for ( auto pp : vprop )
    {
        data.push_back( std::make_pair( pp.first, topoMesh->property( pp.second, heh ) ) );
    }
}

template <typename T>
void copyAttribToCore( TriangleMesh& triMesh, const HandleAndValueVector<T>& data ) {

    for ( auto pp : data )
    {
        auto& attr     = triMesh.getAttrib( pp.first );
        auto& attrData = attr.getDataWithLock();
        attrData.push_back( pp.second );
        attr.unlock();
    }
}

template <typename T>
void copyAttribToWedgeData( const TriangleMesh& mesh,
                            unsigned int vindex,
                            const std::vector<AttribHandle<T>>& attrHandleVec,
                            VectorArray<T>* to ) {
    for ( auto handle : attrHandleVec )
    {
        auto& attr = mesh.getAttrib<T>( handle );
        to->push_back( attr.data()[vindex] );
    }
}

void copyMeshToWedgeData( const TriangleMesh& mesh,
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

TopologicalMesh::TopologicalMesh( const TriangleMesh& triMesh ) {
    //    initWithWedge( triMesh );
    //    return;

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
    std::vector<PropPair<Vector2>> vprop_vec2;
    std::vector<PropPair<Vector3>> vprop_vec3;
    std::vector<PropPair<Vector4>> vprop_vec4;

    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib(
        [&triMesh, this, &vprop_float, &vprop_vec2, &vprop_vec3, &vprop_vec4]( const auto& attr ) {
            // skip builtin attribs
            if ( attr->getName() != std::string( "in_position" ) &&
                 attr->getName() != std::string( "in_normal" ) )
            {
                if ( attr->isFloat() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_float, m_floatPph );
                else if ( attr->isVector2() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec2, m_vec2Pph );
                else if ( attr->isVector3() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec3, m_vec3Pph );
                else if ( attr->isVector4() )
                    addAttribPairToTopo( triMesh, this, attr, vprop_vec4, m_vec4Pph );
                else
                    LOG( logWARNING )
                        << "Warning, mesh attribute " << attr->getName()
                        << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
            }
        } );
    // loop over all attribs and build correspondance pair
    triMesh.vertexAttribs().for_each_attrib(
        [&triMesh, this]( const auto& attr ) {
            if ( attr->getSize() != triMesh.vertices().size() )
            {
                LOG( logWARNING ) << "[TopologicalMesh] Skip badly sized attribute "
                                  << attr->getName();
            }
            else if ( attr->getName() != std::string( "in_position" ) )
            {
                if ( attr->isFloat() )
                {
                    m_wedges.m_wedgeFloatAttribHandles.push_back(
                        triMesh.getAttribHandle<float>( attr->getName() ) );
                    m_wedges.addProp<float>( attr->getName() );
                }
                else if ( attr->isVector2() )
                {
                    m_wedges.m_wedgeVector2AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector2>( attr->getName() ) );
                    m_wedges.addProp<Vector2>( attr->getName() );
                }
                else if ( attr->isVector3() )
                {
                    m_wedges.m_wedgeVector3AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector3>( attr->getName() ) );
                    m_wedges.addProp<Vector3>( attr->getName() );
                }
                else if ( attr->isVector4() )
                {
                    m_wedges.m_wedgeVector4AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector4>( attr->getName() ) );
                    m_wedges.addProp<Vector4>( attr->getName() );
                }
                else
                    LOG( logWARNING )
                        << "Warning, mesh attribute " << attr->getName()
                        << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
            }
        } );

    size_t num_triangles = triMesh.getIndices().size();

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
            const Vector3& n               = triMesh.normals()[inMeshVertexIndex];

            VertexMap::iterator vtr = vertexHandles.find( p );

            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, VertexMap::value_type( p, vh ) );
            }
            else
            { vh = vtr->second; }

            face_vhandles[j]    = vh;
            face_normals[j]     = n;
            face_vertexIndex[j] = inMeshVertexIndex;
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
                set_normal( heh, face_normals[vindex] );
                property( m_inputTriangleMeshIndexPph, heh ) = face_vertexIndex[vindex];
                copyAttribToTopo( triMesh, this, vprop_float, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, this, vprop_vec2, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, this, vprop_vec3, heh, face_vertexIndex[vindex] );
                copyAttribToTopo( triMesh, this, vprop_vec4, heh, face_vertexIndex[vindex] );
                property( m_wedgeIndexPph, heh ) = face_wedges[vindex];
            }
        }
        else
        {
            LOG( logWARNING ) << "Invalid face handle returned : face not added (1)";
            // TODO memorize invalid faces for post processing ...
            //  see
            //  https://www.graphics.rwth-aachen.de/media/openflipper_static/Daily-Builds/Doc/Free/Developer/OBJImporter_8cc_source.html
            // for an exemple of loading
        }
        face_vhandles.clear();
        face_normals.clear();
        face_vertexIndex.clear();
    }
    //    LOG( logINFO ) << "TopologicalMesh: load end with  " << m_wedges.size() << " wedges ";
    //    printWedgesInfo( *this );
}

void TopologicalMesh::initWithWedge( const TriangleMesh& triMesh ) {

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
    triMesh.vertexAttribs().for_each_attrib(
        [&triMesh, this]( const auto& attr ) {
            if ( attr->getSize() != triMesh.vertices().size() )
            {
                LOG( logWARNING ) << "[TopologicalMesh] Skip badly sized attribute "
                                  << attr->getName();
            }
            else if ( attr->getName() != std::string( "in_position" ) )
            {
                if ( attr->isFloat() )
                {
                    m_wedges.m_wedgeFloatAttribHandles.push_back(
                        triMesh.getAttribHandle<float>( attr->getName() ) );
                    m_wedges.addProp<float>( attr->getName() );
                }
                else if ( attr->isVector2() )
                {
                    m_wedges.m_wedgeVector2AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector2>( attr->getName() ) );
                    m_wedges.addProp<Vector2>( attr->getName() );
                }
                else if ( attr->isVector3() )
                {
                    m_wedges.m_wedgeVector3AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector3>( attr->getName() ) );
                    m_wedges.addProp<Vector3>( attr->getName() );
                }
                else if ( attr->isVector4() )
                {
                    m_wedges.m_wedgeVector4AttribHandles.push_back(
                        triMesh.getAttribHandle<Vector4>( attr->getName() ) );
                    m_wedges.addProp<Vector4>( attr->getName() );
                }
                else
                    LOG( logWARNING )
                        << "Warning, mesh attribute " << attr->getName()
                        << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
            }
        } );

    size_t num_triangles = triMesh.getIndices().size();

    for ( size_t i = 0; i < triMesh.vertices().size(); ++i )
    {
        WedgeData wd;
        wd.m_position = triMesh.vertices()[i];
        copyMeshToWedgeData( triMesh,
                             i,
                             m_wedges.m_wedgeFloatAttribHandles,
                             m_wedges.m_wedgeVector2AttribHandles,
                             m_wedges.m_wedgeVector3AttribHandles,
                             m_wedges.m_wedgeVector4AttribHandles,
                             &wd );

        m_wedges.m_data.emplace_back( wd );
    }

    LOG( logINFO ) << "TopologicalMesh: have  " << m_wedges.size() << " wedges ";

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
            const Vector3& n               = triMesh.normals()[inMeshVertexIndex];

            VertexMap::iterator vtr = vertexHandles.find( p );
            TopologicalMesh::VertexHandle vh;
            if ( vtr == vertexHandles.end() )
            {
                vh = add_vertex( p );
                vertexHandles.insert( vtr, VertexMap::value_type( p, vh ) );
            }
            else
            { vh = vtr->second; }

            face_vhandles[j]    = vh;
            face_normals[j]     = n;
            face_vertexIndex[j] = inMeshVertexIndex;
            face_wedges[j]      = WedgeIndex {int( i )};
        }

        // Add the face, then add attribs to vh
        TopologicalMesh::FaceHandle fh = add_face( face_vhandles );
        if ( fh.is_valid() )
        {
            for ( size_t vindex = 0; vindex < face_vhandles.size(); vindex++ )
            {
                TopologicalMesh::HalfedgeHandle heh = halfedge_handle( face_vhandles[vindex], fh );
                set_normal( heh, face_normals[vindex] );
                property( m_wedgeIndexPph, heh ) = m_wedges.newReference( face_wedges[vindex] );
            }
        }
        else
        { LOG( logWARNING ) << "Invalid face handle returned : face not added (2)"; }
        face_vhandles.clear();
        face_normals.clear();
        face_vertexIndex.clear();
    }
    LOG( logINFO ) << "TopologicalMesh: load end with  " << m_wedges.size() << " wedges ";
}

TriangleMesh TopologicalMesh::toTriangleMesh() {
    struct VertexDataInternal {
        Vector3 _vertex;
        Vector3 _normal;

        HandleAndValueVector<float> _float;
        HandleAndValueVector<Vector2> _vec2;
        HandleAndValueVector<Vector3> _vec3;
        HandleAndValueVector<Vector4> _vec4;

        EIGEN_MAKE_ALIGNED_OPERATOR_NEW

        bool operator==( const VertexDataInternal& lhs ) const {
            return _vertex == lhs._vertex && _normal == lhs._normal && _float == lhs._float &&
                   _vec2 == lhs._vec2 && _vec3 == lhs._vec3 && _vec4 == lhs._vec4;
        }
    };

    struct hash_vec {
        size_t operator()( const VertexDataInternal& lvalue ) const {
            size_t hx = std::hash<Scalar>()( lvalue._vertex[0] );
            size_t hy = std::hash<Scalar>()( lvalue._vertex[1] );
            size_t hz = std::hash<Scalar>()( lvalue._vertex[2] );
            return ( hx ^ ( hy << 1 ) ) ^ hz;
        }
    };

    TriangleMesh out;

    using VertexMap = std::unordered_map<VertexDataInternal, int, hash_vec>;

    VertexMap vertexHandles;

    if ( !get_property_handle( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" ) )
    { add_property( m_outputTriangleMeshIndexPph, "OutputTriangleMeshIndices" ); }
    std::vector<PropPair<float>> vprop_float;
    std::vector<PropPair<Vector2>> vprop_vec2;
    std::vector<PropPair<Vector3>> vprop_vec3;
    std::vector<PropPair<Vector4>> vprop_vec4;

    // loop over all attribs and build correspondance pair
    vprop_float.reserve( m_floatPph.size() );
    for ( auto oh : m_floatPph )
        addAttribPairToCore( out, this, oh, vprop_float );
    vprop_vec2.reserve( m_vec2Pph.size() );
    for ( auto oh : m_vec2Pph )
        addAttribPairToCore( out, this, oh, vprop_vec2 );
    vprop_vec3.reserve( m_vec3Pph.size() );
    for ( auto oh : m_vec3Pph )
        addAttribPairToCore( out, this, oh, vprop_vec3 );
    vprop_vec4.reserve( m_vec4Pph.size() );
    for ( auto oh : m_vec4Pph )
        addAttribPairToCore( out, this, oh, vprop_vec4 );

    // iterator over all faces
    unsigned int vertexIndex = 0;

    // out will have at least n_vertices vertices and normals.
    TriangleMesh::PointAttribHandle::Container vertices;
    TriangleMesh::NormalAttribHandle::Container normals;
    TriangleMesh::IndexContainerType indices;

    vertices.reserve( n_vertices() );
    normals.reserve( n_vertices() );
    indices.reserve( n_faces() );

    for ( TopologicalMesh::FaceIter f_it = faces_sbegin(); f_it != faces_end(); ++f_it )
    {
        int tindices[3];
        int i = 0;

        // iterator over vertex (through halfedge to get access to halfedge normals)
        for ( TopologicalMesh::ConstFaceHalfedgeIter fh_it = cfh_iter( *f_it ); fh_it.is_valid();
              ++fh_it )
        {
            VertexDataInternal v;
            CORE_ASSERT( i < 3, "Non-triangular face found." );
            v._vertex = point( to_vertex_handle( *fh_it ) );
            v._normal = normal( to_vertex_handle( *fh_it ), *f_it );

            copyAttribToCoreVertex( v._float, this, vprop_float, *fh_it );
            copyAttribToCoreVertex( v._vec2, this, vprop_vec2, *fh_it );
            copyAttribToCoreVertex( v._vec3, this, vprop_vec3, *fh_it );
            copyAttribToCoreVertex( v._vec4, this, vprop_vec4, *fh_it );

            int vi;
            VertexMap::iterator vtr = vertexHandles.find( v );
            if ( vtr == vertexHandles.end() )
            {
                vi = int( vertexIndex++ );
                vertexHandles.insert( vtr, VertexMap::value_type( v, vi ) );
                vertices.push_back( v._vertex );
                normals.push_back( v._normal );

                copyAttribToCore( out, v._float );
                copyAttribToCore( out, v._vec2 );
                copyAttribToCore( out, v._vec3 );
                copyAttribToCore( out, v._vec4 );
            }
            else
            { vi = vtr->second; }
            tindices[i]                                      = vi;
            property( m_outputTriangleMeshIndexPph, *fh_it ) = vi;
            i++;
        }
        indices.emplace_back( tindices[0], tindices[1], tindices[2] );
    }
    out.setVertices( std::move( vertices ) );
    out.setNormals( std::move( normals ) );
    out.setIndices( std::move( indices ) );
    CORE_ASSERT( vertexIndex == vertices.size(),
                 "Inconsistent number of faces in generated TriangleMesh." );

    return out;
}

template <typename T>
void copyWedgeDataToAttribContainer( AlignedStdVector<typename Attrib<T>::Container>& c,
                                     const VectorArray<T>& wd ) {
    for ( size_t i = 0; i < wd.size(); ++i )
    {
        c[i].push_back( wd[i] );
    }
}

template <typename T>
void moveContainerToMesh( TriangleMesh& out,
                          const std::vector<std::string>& names,
                          AlignedStdVector<typename Attrib<T>::Container>& wedgeAttribData ) {
    for ( size_t i = 0; i < wedgeAttribData.size(); ++i )
    {
        auto attrHandle = out.addAttrib<T>( names[i] );
        out.getAttrib( attrHandle ).setData( std::move( wedgeAttribData[i] ) );
    }
}

TriangleMesh TopologicalMesh::toTriangleMeshFromWedges() {
    // first cleanup deleted element
    garbage_collection();

    TriangleMesh out;
    TriangleMesh::IndexContainerType indices;

    /// add attribs to out
    std::vector<AttribHandle<float>> wedgeFloatAttribHandles;
    std::vector<AttribHandle<Vector2>> wedgeVector2AttribHandles;
    std::vector<AttribHandle<Vector3>> wedgeVector3AttribHandles;
    std::vector<AttribHandle<Vector4>> wedgeVector4AttribHandles;

    TriangleMesh::PointAttribHandle::Container wedgePosition;
    AlignedStdVector<Attrib<float>::Container> wedgeFloatAttribData(
        m_wedges.m_floatAttribNames.size() );
    AlignedStdVector<Attrib<Vector2>::Container> wedgeVector2AttribData(
        m_wedges.m_vector2AttribNames.size() );
    AlignedStdVector<Attrib<Vector3>::Container> wedgeVector3AttribData(
        m_wedges.m_vector3AttribNames.size() );
    AlignedStdVector<Attrib<Vector4>::Container> wedgeVector4AttribData(
        m_wedges.m_vector4AttribNames.size() );

    /// Wedges are output vertices !
    for ( WedgeIndex widx {0}; widx < WedgeIndex( m_wedges.size() ); ++widx )
    {
        const auto& wd = m_wedges.getWedgeData( widx );
        wedgePosition.push_back( wd.m_position );
        copyWedgeDataToAttribContainer( wedgeFloatAttribData, wd.m_floatAttrib );
        copyWedgeDataToAttribContainer( wedgeVector2AttribData, wd.m_vector2Attrib );
        copyWedgeDataToAttribContainer( wedgeVector3AttribData, wd.m_vector3Attrib );
        copyWedgeDataToAttribContainer( wedgeVector4AttribData, wd.m_vector4Attrib );
    }

    out.setVertices( std::move( wedgePosition ) );
    moveContainerToMesh<float>( out, m_wedges.m_floatAttribNames, wedgeFloatAttribData );
    moveContainerToMesh<Vector2>( out, m_wedges.m_vector2AttribNames, wedgeVector2AttribData );
    moveContainerToMesh<Vector3>( out, m_wedges.m_vector3AttribNames, wedgeVector3AttribData );
    moveContainerToMesh<Vector4>( out, m_wedges.m_vector4AttribNames, wedgeVector4AttribData );

    for ( TopologicalMesh::FaceIter f_it = faces_sbegin(); f_it != faces_end(); ++f_it )
    {
        int tindices[3];
        int i = 0;

        // iterator over vertex (through halfedge to get access to halfedge normals)
        for ( TopologicalMesh::ConstFaceHalfedgeIter fh_it = cfh_iter( *f_it ); fh_it.is_valid();
              ++fh_it )
        {
            CORE_ASSERT( i < 3, "Non-triangular face found." );
            tindices[i] = property( m_wedgeIndexPph, *fh_it );
            i++;
        }
        indices.emplace_back( tindices[0], tindices[1], tindices[2] );
    }

    out.setIndices( std::move( indices ) );

    return out;
}

void TopologicalMesh::updateTriangleMesh( Ra::Core::Geometry::TriangleMesh& /*mesh*/ ) {
    CORE_ASSERT( false, "not implemented yet" );
    ///\todo ;)
}

bool TopologicalMesh::splitEdge( TopologicalMesh::EdgeHandle eh, Scalar f ) {
    // Global schema of operation
    /*
               TRIANGLES ONLY
         before                after
               A                       A
            / F0 \                / F2 | F0 \
           /      \              /     |     \
          /h1    h0\            /h1  e2|e0  h0\
         /    he0   \          /  he2  |  he0  \
        V1 -------- V0       V1 ------ V ------ V0
         \    he1   /          \  he3  |  he1  /
          \o1    o0/            \o1  e3|e1  o0/
           \      /              \     |     /
            \ F1 /                \ F3 | F1 /
               B                       B

    */

    // incorrect factor
    if ( f < 0 || f > 1 ) { return false; }

    // get existing topology data
    HalfedgeHandle he0 = halfedge_handle( eh, 0 );
    HalfedgeHandle he1 = halfedge_handle( eh, 1 );
    VertexHandle v0    = to_vertex_handle( he0 );
    VertexHandle v1    = to_vertex_handle( he1 );
    FaceHandle F0      = face_handle( he0 );
    FaceHandle F1      = face_handle( he1 );

    // not triangles or holes
    if ( ( !is_boundary( he0 ) && valence( F0 ) != 3 ) ||
         ( !is_boundary( he1 ) && valence( F1 ) != 3 ) )
    { return false; }

    // add the new vertex
    const Point p  = Point( f * point( v0 ) + ( Scalar( 1. ) - f ) * point( v1 ) );
    VertexHandle v = add_vertex( p );

    // create the new faces and reconnect the topology
    HalfedgeHandle he3 = new_edge( v, v1 );
    HalfedgeHandle he2 = opposite_halfedge_handle( he3 );
    set_halfedge_handle( v, he0 );
    set_vertex_handle( he1, v );

    // does F0 exist
    if ( !is_boundary( he0 ) )
    {
        HalfedgeHandle h0 = next_halfedge_handle( he0 );
        HalfedgeHandle h1 = next_halfedge_handle( h0 );
        // create new edge
        VertexHandle A    = to_vertex_handle( h0 );
        HalfedgeHandle e2 = new_edge( v, A );
        HalfedgeHandle e0 = opposite_halfedge_handle( e2 );
        // split F0
        FaceHandle F2 = new_face();
        set_halfedge_handle( F0, he0 );
        set_halfedge_handle( F2, h1 );
        // update F0
        set_face_handle( h0, F0 );
        set_face_handle( e0, F0 );
        set_face_handle( he0, F0 );
        set_next_halfedge_handle( he0, h0 );
        set_next_halfedge_handle( h0, e0 );
        set_next_halfedge_handle( e0, he0 );
        // update F2
        set_face_handle( h1, F2 );
        set_face_handle( he2, F2 );
        set_face_handle( e2, F2 );
        set_next_halfedge_handle( e2, h1 );
        set_next_halfedge_handle( h1, he2 );
        set_next_halfedge_handle( he2, e2 );
        // deal with custom properties
        // interpolate at he2
        interpolateAllProps( h1, he0, he2, 0.5 );
        // copy at e0, and e2
        copyAllProps( he2, e0 );
    }
    else
    {
        HalfedgeHandle h1 = prev_halfedge_handle( he0 );
        set_next_halfedge_handle( h1, he2 );
        set_next_halfedge_handle( he2, he0 );
        // next halfedge handle of he0 already is h0
        // halfedge handle of V already is he0
    }

    // does F1 exist
    if ( !is_boundary( he1 ) )
    {
        HalfedgeHandle o1 = next_halfedge_handle( he1 );
        HalfedgeHandle o0 = next_halfedge_handle( o1 );
        // create new edge
        VertexHandle B    = to_vertex_handle( o1 );
        HalfedgeHandle e1 = new_edge( v, B );
        HalfedgeHandle e3 = opposite_halfedge_handle( e1 );
        // split F1
        FaceHandle F3 = new_face();
        set_halfedge_handle( F3, o1 );
        set_halfedge_handle( F1, he1 );
        // update F1
        set_face_handle( o1, F3 );
        set_face_handle( e3, F3 );
        set_face_handle( he3, F3 );
        set_next_halfedge_handle( he3, o1 );
        set_next_halfedge_handle( o1, e3 );
        set_next_halfedge_handle( e3, he3 );
        // update F3
        set_face_handle( o0, F1 );
        set_face_handle( he1, F1 );
        set_face_handle( e1, F1 );
        set_next_halfedge_handle( he1, e1 );
        set_next_halfedge_handle( e1, o0 );
        set_next_halfedge_handle( o0, he1 );
        // deal with custom properties
        // first copy at he3
        copyAllProps( he1, he3 );
        // interpolate at he1
        interpolateAllProps( o0, he3, he1, 0.5 );
        // copy at e1, and e3
        copyAllProps( he1, e3 );
        copyAllProps( o1, e1 );
    }
    else
    {
        HalfedgeHandle o1 = next_halfedge_handle( he1 );
        // next halfedge handle of o0 already is he1
        set_next_halfedge_handle( he1, he3 );
        set_next_halfedge_handle( he3, o1 );
        // halfedge handle of V already is he0
    }

    // ensure consistency at v1
    if ( halfedge_handle( v1 ) == he0 ) { set_halfedge_handle( v1, he2 ); }

    return true;
}

template <typename T>
void interpolate( const VectorArray<T>& in1,
                  const VectorArray<T>& in2,
                  VectorArray<T>& out,
                  const Scalar alpha ) {
    for ( size_t i = 0; i < in1.size(); ++i )
    {
        out.push_back( ( 1_ra - alpha ) * in1[i] + alpha * in2[i] );
    }
}

TopologicalMesh::WedgeData
TopologicalMesh::interpolateWedgeAttributes( const TopologicalMesh::WedgeData& w1,
                                             const TopologicalMesh::WedgeData& w2,
                                             Scalar alpha ) {
    WedgeData ret;
    interpolate( w1.m_floatAttrib, w2.m_floatAttrib, ret.m_floatAttrib, alpha );
    interpolate( w1.m_vector2Attrib, w2.m_vector2Attrib, ret.m_vector2Attrib, alpha );
    interpolate( w1.m_vector3Attrib, w2.m_vector3Attrib, ret.m_vector3Attrib, alpha );
    interpolate( w1.m_vector4Attrib, w2.m_vector4Attrib, ret.m_vector4Attrib, alpha );
    return ret;
}

//-----------------------------------------------------------------------------
// from /OpenMesh/Core/Mesh/TriConnectivity.cc
void TopologicalMesh::split( EdgeHandle _eh, VertexHandle _vh ) {
    HalfedgeHandle h0 = halfedge_handle( _eh, 0 );
    HalfedgeHandle o0 = halfedge_handle( _eh, 1 );

    VertexHandle v2 = to_vertex_handle( o0 );

    HalfedgeHandle e1 = new_edge( _vh, v2 );
    HalfedgeHandle t1 = opposite_halfedge_handle( e1 );

    FaceHandle f0 = face_handle( h0 );
    FaceHandle f3 = face_handle( o0 );

    set_halfedge_handle( _vh, h0 );
    set_vertex_handle( o0, _vh );

    if ( !is_boundary( h0 ) )
    {
        HalfedgeHandle h1 = next_halfedge_handle( h0 );
        HalfedgeHandle h2 = next_halfedge_handle( h1 );

        VertexHandle v1 = to_vertex_handle( h1 );

        HalfedgeHandle e0 = new_edge( _vh, v1 );
        HalfedgeHandle t0 = opposite_halfedge_handle( e0 );

        FaceHandle f1 = new_face();
        set_halfedge_handle( f0, h0 );
        set_halfedge_handle( f1, h2 );

        set_face_handle( h1, f0 );
        set_face_handle( t0, f0 );
        set_face_handle( h0, f0 );

        set_face_handle( h2, f1 );
        set_face_handle( t1, f1 );
        set_face_handle( e0, f1 );

        set_next_halfedge_handle( h0, h1 );
        set_next_halfedge_handle( h1, t0 );
        set_next_halfedge_handle( t0, h0 );

        set_next_halfedge_handle( e0, h2 );
        set_next_halfedge_handle( h2, t1 );
        set_next_halfedge_handle( t1, e0 );
    }
    else
    {
        set_next_halfedge_handle( prev_halfedge_handle( h0 ), t1 );
        set_next_halfedge_handle( t1, h0 );
        // halfedge handle of _vh already is h0
    }

    if ( !is_boundary( o0 ) )
    {
        HalfedgeHandle o1 = next_halfedge_handle( o0 );
        HalfedgeHandle o2 = next_halfedge_handle( o1 );

        VertexHandle v3 = to_vertex_handle( o1 );

        HalfedgeHandle e2 = new_edge( _vh, v3 );
        HalfedgeHandle t2 = opposite_halfedge_handle( e2 );

        FaceHandle f2 = new_face();
        set_halfedge_handle( f2, o1 );
        set_halfedge_handle( f3, o0 );

        set_face_handle( o1, f2 );
        set_face_handle( t2, f2 );
        set_face_handle( e1, f2 );

        set_face_handle( o2, f3 );
        set_face_handle( o0, f3 );
        set_face_handle( e2, f3 );

        set_next_halfedge_handle( e1, o1 );
        set_next_halfedge_handle( o1, t2 );
        set_next_halfedge_handle( t2, e1 );

        set_next_halfedge_handle( o0, e2 );
        set_next_halfedge_handle( e2, o2 );
        set_next_halfedge_handle( o2, o0 );
    }
    else
    {
        set_next_halfedge_handle( e1, next_halfedge_handle( o0 ) );
        set_next_halfedge_handle( o0, e1 );
        set_halfedge_handle( _vh, e1 );
    }

    if ( halfedge_handle( v2 ) == h0 ) set_halfedge_handle( v2, t1 );
}

//-----------------------------------------------------------------------------

void TopologicalMesh::split_copy( EdgeHandle _eh, VertexHandle _vh ) {
    const VertexHandle v0 = to_vertex_handle( halfedge_handle( _eh, 0 ) );
    const VertexHandle v1 = to_vertex_handle( halfedge_handle( _eh, 1 ) );

    const int nf = n_faces();

    // Split the halfedge ( handle will be preserved)
    split( _eh, _vh );

    // Copy the properties of the original edge to all neighbor edges that
    // have been created
    for ( VEIter ve_it = ve_iter( _vh ); ve_it.is_valid(); ++ve_it )
        copy_all_properties( _eh, *ve_it, true );

    for ( auto vh : {v0, v1} )
    {
        // get the halfedge pointing from new vertex to old vertex
        const HalfedgeHandle h = find_halfedge( _vh, vh );
        if ( !is_boundary(
                 h ) ) // for boundaries there are no faces whose properties need to be copied
        {
            FaceHandle fh0 = face_handle( h );
            FaceHandle fh1 = face_handle( opposite_halfedge_handle( prev_halfedge_handle( h ) ) );
            if ( fh0.idx() >= nf ) // is fh0 the new face?
                std::swap( fh0, fh1 );

            // copy properties from old face to new face
            copy_all_properties( fh0, fh1, true );
        }
    }
}

bool TopologicalMesh::splitEdgeWedge( TopologicalMesh::EdgeHandle eh, Scalar f ) {
    // Global schema of operation
    /*
               TRIANGLES ONLY
         before                after

            /    \                /    |    \
           /      \              /     |     \
          /h2    h1\            /r2  r1|s2  s1\
         /    h0 -->\          /  r0   | s0=h0 \
        V0 -------- V1        V0-(1-f)-V---f---V1
         \<-- o0    /          \  u0   | t0=o0 /
          \o1    o2/            \u1  u2|t1  t2/
           \      /              \     |     /
            \    /                \    |    /


    */

    //   LOG( logINFO ) << "* before ************************";
    //   printWedgesInfo( *this );

    // incorrect factor
    if ( f < 0 || f > 1 ) { return false; }

    if ( is_boundary( eh ) )
    {
        CORE_ASSERT( false, "ouch" );
        return false;
    }

    const auto h0 = halfedge_handle( eh, 0 );
    const auto o0 = halfedge_handle( eh, 1 );

    const auto v0 = to_vertex_handle( o0 );
    const auto v1 = to_vertex_handle( h0 );

    //   LOG( logINFO ) << "* split **** " << v0 << " ----> " << v1;

    // add the new point
    const Point p   = Point( f * point( v1 ) + ( 1_ra - f ) * point( v0 ) );
    VertexHandle vh = add_vertex( p );

    const auto h1 = next_halfedge_handle( h0 );
    const auto h2 = next_halfedge_handle( h1 );

    const auto o1 = next_halfedge_handle( o0 );
    const auto o2 = next_halfedge_handle( o1 );

    // compute interpolated wedge, or the two wedges if not the same wedges
    // around the two vertices of the edge (we always compute for two wedges,
    // even if add will return the same wedge.
    const auto hw0idx = property( m_wedgeIndexPph, h2 );
    const auto hw1idx = property( m_wedgeIndexPph, h0 );
    const auto ow0idx = property( m_wedgeIndexPph, o2 );
    const auto ow1idx = property( m_wedgeIndexPph, o0 );
    const auto hw0    = m_wedges.getWedgeData( hw0idx );
    const auto hw1    = m_wedges.getWedgeData( hw1idx );
    const auto ow0    = m_wedges.getWedgeData( ow0idx );
    const auto ow1    = m_wedges.getWedgeData( ow1idx );
    auto hvw          = interpolateWedgeAttributes( hw0, hw1, f );
    auto ovw          = interpolateWedgeAttributes( ow1, ow0, f );
    hvw.m_position    = p;
    ovw.m_position    = p;
    auto hvwidx       = m_wedges.add( hvw );
    auto ovwidx       = m_wedges.add( ovw );

    split_copy( eh, vh );

    auto r0 = find_halfedge( v0, vh );
    auto s0 = find_halfedge( vh, v1 );
    auto t0 = find_halfedge( v1, vh );
    auto u0 = find_halfedge( vh, v0 );

    auto r1 = next_halfedge_handle( r0 );
    auto r2 = next_halfedge_handle( r1 );

    auto s1 = next_halfedge_handle( s0 );
    auto s2 = next_halfedge_handle( s1 );

    auto t1 = next_halfedge_handle( t0 );
    auto t2 = next_halfedge_handle( t1 );

    auto u1 = next_halfedge_handle( u0 );
    auto u2 = next_halfedge_handle( u1 );

    CORE_ASSERT( s0 == h0, "TopologicalMesh: splitEdgeWedge inconsistency" );
    CORE_ASSERT( t0 == o0, "TopologicalMesh: splitEdgeWedge inconsistency" );

    auto updateWedgeIndex1 = [this]( WedgeIndex widx_,
                                     HalfedgeHandle r0_,
                                     HalfedgeHandle r1_,
                                     HalfedgeHandle r2_,
                                     HalfedgeHandle h1_,
                                     HalfedgeHandle h2_ ) {
        CORE_UNUSED( r2_ );
        CORE_UNUSED( h2_ );

        if ( !is_boundary( r0_ ) )
        {
            CORE_ASSERT( r2_ == h2_, "TopologicalMesh: splitEdgeWedge inconsistency" );

            // Increment here, the first reference is for the other he
            property( this->m_wedgeIndexPph, r0_ ) = this->m_wedges.newReference( widx_ );
            property( this->m_wedgeIndexPph, r1_ ) =
                this->m_wedges.newReference( property( this->m_wedgeIndexPph, h1_ ) );
        }
        else
        { property( this->m_wedgeIndexPph, r0_ ) = WedgeIndex(); }
    };

    auto updateWedgeIndex2 = [this]( WedgeIndex widx_,
                                     HalfedgeHandle s0_,
                                     HalfedgeHandle s1_,
                                     HalfedgeHandle s2_,
                                     HalfedgeHandle h0_,
                                     HalfedgeHandle h1_ ) {
        CORE_UNUSED( s1_ );
        CORE_UNUSED( h1_ );

        if ( !is_boundary( s0_ ) )
        {
            CORE_ASSERT( s1_ == h1_, "TopologicalMesh: splitEdgeWedge inconsistency" );
            // do not increment here, since add has set ref to 1
            property( this->m_wedgeIndexPph, s2_ ) = widx_;
            // "steal" ref from previous he (actually s0 is h0, u0 is the steal
            // from o0.
            property( this->m_wedgeIndexPph, s0_ ) = property( this->m_wedgeIndexPph, h0_ );
        }
        else
        { property( this->m_wedgeIndexPph, s0_ ) = WedgeIndex(); }
    };

    // this update read from o0, must be done before t which reads from o0
    updateWedgeIndex2( ovwidx, u0, u1, u2, o0, o1 );
    // those update might be in any order
    updateWedgeIndex2( hvwidx, s0, s1, s2, h0, h1 );
    updateWedgeIndex1( hvwidx, r0, r1, r2, h1, h2 );
    updateWedgeIndex1( ovwidx, t0, t1, t2, o1, o2 );

    // LOG( logINFO ) << "* after ************************";
    //  printWedgesInfo( *this );

    return true;
}

void TopologicalMesh::collapseWedge( TopologicalMesh::HalfedgeHandle heh ) {
    HalfedgeHandle h  = heh;
    HalfedgeHandle hn = next_halfedge_handle( h );
    HalfedgeHandle hp = prev_halfedge_handle( h );

    HalfedgeHandle o  = opposite_halfedge_handle( h );
    HalfedgeHandle on = next_halfedge_handle( o );
    HalfedgeHandle op = prev_halfedge_handle( o );

    //    FaceHandle fh = face_handle( h );
    //    FaceHandle fo = face_handle( o );

    VertexHandle vh = to_vertex_handle( h );
    VertexHandle vo = to_vertex_handle( o );

    auto position = m_wedges.getWedgeData( property( m_wedgeIndexPph, heh ) ).m_position;
    auto widx     = property( m_wedgeIndexPph, heh );

    CORE_ASSERT( widx.isValid(), "try to collapse onto an invalid wedge" );
    CORE_ASSERT( !isFeatureVertex( vo ), "try to collapse a feature vertex" );

    for ( VertexIHalfedgeIter vih_it( vih_iter( vo ) ); vih_it.is_valid(); ++vih_it )
    {
        // delete and set to new widx
        m_wedges.del( property( m_wedgeIndexPph, *vih_it ) );
        property( m_wedgeIndexPph, *vih_it ) = m_wedges.newReference( widx );
    }
    // but remove one ref for the deleted opposite he
    m_wedges.del( property( m_wedgeIndexPph, o ) );

    // and delete wedge of the remove he
    // first if h is not boundary, copy the wedgeIndex of hn to hp to it
    if ( !is_boundary( h ) )
    {
        property( m_wedgeIndexPph, hp ) =
            m_wedges.newReference( property( m_wedgeIndexPph, opposite_halfedge_handle( hn ) ) );
    }
    m_wedges.del( property( m_wedgeIndexPph, hn ) );
    m_wedges.del( property( m_wedgeIndexPph, opposite_halfedge_handle( hn ) ) );

    if ( !is_boundary( o ) )
    {
        property( m_wedgeIndexPph, on ) =
            m_wedges.newReference( property( m_wedgeIndexPph, opposite_halfedge_handle( op ) ) );
    }
    m_wedges.del( property( m_wedgeIndexPph, op ) );
    m_wedges.del( property( m_wedgeIndexPph, opposite_halfedge_handle( op ) ) );

    base::collapse( h );

    for ( VertexIHalfedgeIter vih_it( vih_iter( vh ) ); vih_it.is_valid(); ++vih_it )
    {
        // delete and set to new widx
        m_wedges.setWedgePosition( property( m_wedgeIndexPph, *vih_it ), position );
    }
}

void TopologicalMesh::garbage_collection() {
    for ( HalfedgeIter he_it = halfedges_begin(); he_it != halfedges_end(); ++he_it )
    {
        // already done in collapseWedge
        // if ( status( *he_it ).deleted() ) { m_wedges.del(property(
        // m_wedgeIndexPph, *he_it )); }
    }

    auto offset = m_wedges.computeCleanupOffset();
    for ( HalfedgeIter he_it = halfedges_begin(); he_it != halfedges_end(); ++he_it )
    {
        if ( !status( *he_it ).deleted() )
        {
            auto index = property( m_wedgeIndexPph, *he_it );
            if ( index.isValid() ) { property( m_wedgeIndexPph, *he_it ) = index - offset[index]; }
        }
    }
    m_wedges.garbageCollection();
    base::garbage_collection();

    for ( HalfedgeIter he_it = halfedges_begin(); he_it != halfedges_end(); ++he_it )
    {
        ON_ASSERT( auto idx = property( m_wedgeIndexPph, *he_it ); );
        CORE_ASSERT( !idx.isValid() || !m_wedges.getWedge( idx ).isDeleted(),
                     "references deleted wedge remains after garbage collection" );
    }
    for ( size_t i = 0; i < m_wedges.size(); ++i )
    {
        CORE_ASSERT( !m_wedges.getWedge( WedgeIndex( i ) ).isDeleted(),
                     "deleted wedge remains after garbage collection" );
    }
}

void TopologicalMesh::delete_face( FaceHandle _fh, bool _delete_isolated_vertices ) {
    for ( auto itr = fh_begin( _fh ); itr.is_valid(); ++itr )
    {
        auto idx = property( m_wedgeIndexPph, *itr );
        CORE_ASSERT( idx.isValid(), "delete face, halfedge has an invalid wedge index" );
        m_wedges.del( idx );
        // set an invalid index for the boundary halfedges
        property( m_wedgeIndexPph, *itr ) = WedgeIndex();
    }
    base::delete_face( _fh, _delete_isolated_vertices );
}

/////////////// WEDGES RELATED STUFF /////////////////

TopologicalMesh::WedgeIndex
TopologicalMesh::WedgeCollection::add( const TopologicalMesh::WedgeData& wd ) {
    WedgeIndex idx;
    auto itr = std::find( m_data.begin(), m_data.end(), Wedge {wd} );

    if ( itr == m_data.end() )
    {
        idx = m_data.size();
        m_data.emplace_back( wd );
    }
    else
    {
        itr->incrementRefCount();
        idx = std::distance( m_data.begin(), itr );
    }
    return idx;
}

std::vector<int> TopologicalMesh::WedgeCollection::computeCleanupOffset() const {
    std::vector<int> ret( m_data.size(), 0 );
    int currentOffset = 0;
    for ( size_t i = 0; i < m_data.size(); ++i )
    {
        if ( m_data[i].isDeleted() )
        {
            ++currentOffset;
            ret[i] = -1;
        }
        else
        { ret[i] = currentOffset; }
    }
    return ret;
}

} // namespace Geometry
} // namespace Core
} // namespace Ra
