#include <Core/Geometry/IndexedGeometry.hpp>
#include <iterator>

namespace Ra {
namespace Core {
namespace Geometry {

MultiIndexedGeometry::MultiIndexedGeometry( const MultiIndexedGeometry& other ) :
    AttribArrayGeometry( other ), m_indices( other.m_indices ) {}

/// \fixme Deep copy
MultiIndexedGeometry::MultiIndexedGeometry( MultiIndexedGeometry&& other ) :
    AttribArrayGeometry( std::move( other ) ), m_indices( std::move( other.m_indices ) ) {}

MultiIndexedGeometry::MultiIndexedGeometry( const AttribArrayGeometry& other ) :
    AttribArrayGeometry( other ) {}

MultiIndexedGeometry::MultiIndexedGeometry( AttribArrayGeometry&& other ) :
    AttribArrayGeometry( std::move( other ) ) {}

/// \fixme Deep copy
MultiIndexedGeometry& MultiIndexedGeometry::operator=( const MultiIndexedGeometry& other ) {
    AttribArrayGeometry::operator=( other );
    m_indices                    = other.m_indices;
    notify();
    return *this;
}

/// \fixme Deep copy
MultiIndexedGeometry& MultiIndexedGeometry::operator=( MultiIndexedGeometry&& other ) {
    AttribArrayGeometry::operator=( std::move( other ) );
    m_indices                    = std::move( other.m_indices );
    notify();
    return *this;
}

void MultiIndexedGeometry::clear() {
    m_indices.clear();
    AttribArrayGeometry::clear();
    notify();
}

/// \fixme Deep copy
void MultiIndexedGeometry::copy( const MultiIndexedGeometry& other ) {
    AttribArrayGeometry::copyBaseGeometry( other );
    m_indices = other.m_indices;
    notify();
}

/// \todo Implement MultiIndexedGeometry::checkConsistency
void MultiIndexedGeometry::checkConsistency() const {
#ifdef CORE_DEBUG
#endif
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool MultiIndexedGeometry::containsLayer( const LayerSemantic& semanticName ) const {
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first.find( semanticName ) != key.first.end() ) return true;
    }
    return false;
}

bool MultiIndexedGeometry::containsLayer( const LayerSemanticCollection& semantics ) const {
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first == semantics ) return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

size_t MultiIndexedGeometry::countLayers( const LayerSemantic& semanticName ) const {
    size_t c = 0;
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first.find( semanticName ) != key.first.end() ) ++c;
    }
    return c;
}

size_t MultiIndexedGeometry::countLayers( const LayerSemanticCollection& semantics ) const {
    size_t c = 0;
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first == semantics ) ++c;
    }
    return c;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

const GeometryIndexLayerBase&
MultiIndexedGeometry::getFirstLayerOccurrence( const LayerSemantic& semanticName ) const {
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first.find( semanticName ) != key.first.end() ) return *( value.second );
    }
    throw std::out_of_range( "Layer entry not found" );
}

const GeometryIndexLayerBase&
MultiIndexedGeometry::getFirstLayerOccurrence( const LayerSemanticCollection& semantics ) const {
    for ( const auto& [key, value] : m_indices )
    {
        if ( key.first == semantics ) return *( value.second );
    }
    throw std::out_of_range( "Layer entry not found" );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

GeometryIndexLayerBase&
MultiIndexedGeometry::getFirstLayerOccurrenceWithLock( const LayerSemantic& semanticName ) {
    for ( auto& [key, value] : m_indices )
    {
        if ( key.first.find( semanticName ) != key.first.end() )
        {
            CORE_ASSERT( !value.first, "try to get already locked layer" );
            value.first = true;
            return *( value.second );
        }
    }
    throw std::out_of_range( "Layer entry not found" );
}

GeometryIndexLayerBase&
MultiIndexedGeometry::getFirstLayerOccurrenceWithLock( const LayerSemanticCollection& semantics ) {
    for ( auto& [key, value] : m_indices )
    {
        if ( key.first == semantics )
        {
            CORE_ASSERT( !value.first, "try to get already locked layer" );
            value.first = true;
            return *( value.second );
        }
    }
    throw std::out_of_range( "Layer entry not found" );
}

GeometryIndexLayerBase& MultiIndexedGeometry::getLayerWithLock( const LayerKeyType& layerKey ) {
    auto& p = m_indices.at( layerKey );
    CORE_ASSERT( !p.first, "try to get already locked layer" );
    p.first = true;
    return *( p.second );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void MultiIndexedGeometry::unlockFirstLayerOccurrence( const LayerSemantic& semanticName ) {
    for ( auto& [key, value] : m_indices )
    {
        if ( key.first.find( semanticName ) != key.first.end() )
        {
            CORE_ASSERT( value.first, "try to release unlocked layer" );
            value.first = false;
            notify();
        }
    }
    throw std::out_of_range( "Layer entry not found" );
}

void MultiIndexedGeometry::unlockFirstLayerOccurrence( const LayerSemanticCollection& semantics ) {
    for ( auto& [key, value] : m_indices )
    {
        if ( key.first == semantics )
        {
            CORE_ASSERT( value.first, "try to release unlocked layer" );
            value.first = false;
            notify();
        }
    }
    throw std::out_of_range( "Layer entry not found" );
}

void MultiIndexedGeometry::unlockLayer( const LayerKeyType& layerKey ) {
    auto& p = m_indices.at( layerKey );
    CORE_ASSERT( p.first, "try to release unlocked layer" );
    p.first = true;
    notify();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

bool MultiIndexedGeometry::addLayer( std::unique_ptr<GeometryIndexLayerBase>&& layer,
                                     const std::string& layerName ) {
    LayerKeyType key = std::make_pair( layer->semantics(), layerName );
    if ( m_indices.find( key ) != m_indices.end() ) return false;

    m_indices.insert( {key, std::make_pair( false, layer.release() )} );

    notify();
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

std::size_t MultiIndexedGeometry::KeyHash::operator()( const LayerKeyType& k ) const {
    // Mix semantic collection into a single identifier string
    std::ostringstream stream;
    std::copy( k.first.begin(), k.first.end(), std::ostream_iterator<std::string>( stream, "" ) );
    std::string result = stream.str();
    std::sort( result.begin(), result.end() );

    // Combine with layer name hash
    return std::hash<std::string> {}( result ) ^ ( std::hash<std::string> {}( k.second ) << 1 );
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

void PointCloudIndexLayer::generateIndicesFromAttributes( const AttribArrayGeometry& attr ) {
    auto nbVert = attr.vertices().size();
    collection().resize( nbVert );
    collection().getMap() = IndexContainerType::Matrix::LinSpaced( nbVert, 0, nbVert - 1 );
}

} // namespace Geometry
} // namespace Core
} // namespace Ra