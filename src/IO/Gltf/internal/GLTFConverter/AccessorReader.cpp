#include <Core/Asset/FileData.hpp>

#include <IO/Gltf/internal/GLTFConverter/AccessorReader.hpp>

namespace Ra {
namespace IO {
namespace GLTF {

using namespace fx;

using namespace Ra::Core::Utils;

const std::map<gltf::Accessor::ComponentType, int> nbByteByValueMap {
    { gltf::Accessor::ComponentType::Byte, 1 },
    { gltf::Accessor::ComponentType::UnsignedByte, 1 },
    { gltf::Accessor::ComponentType::Short, 2 },
    { gltf::Accessor::ComponentType::UnsignedShort, 2 },
    { gltf::Accessor::ComponentType::UnsignedByte, 4 },
    { gltf::Accessor::ComponentType::Float, 4 },
    { gltf::Accessor::ComponentType::None, 0 } };

const std::map<gltf::Accessor::Type, int> nbValueByComponentMap {
    { gltf::Accessor::Type::Scalar, 1 },
    { gltf::Accessor::Type::Vec2, 2 },
    { gltf::Accessor::Type::Vec3, 3 },
    { gltf::Accessor::Type::Vec4, 4 },
    { gltf::Accessor::Type::Mat2, 4 },
    { gltf::Accessor::Type::Mat3, 9 },
    { gltf::Accessor::Type::Mat4, 16 },
    { gltf::Accessor::Type::None, 0 } };

template <typename T>
void minmax( uint8_t* data, const gltf::Accessor& accessor, int nbComponents ) {
    std::vector<T> min;
    std::vector<T> max;
    T defaultMin       = std::numeric_limits<T>::min();
    T defaultMax       = std::numeric_limits<T>::max();
    auto convertedData = reinterpret_cast<T*>( data );
    if ( accessor.min.empty() ) { min = std::vector<T>( nbComponents, defaultMin ); }
    else {
        std::transform( accessor.min.begin(),
                        accessor.min.end(),
                        std::back_inserter( min ),
                        []( float i ) { return static_cast<T>( i ); } );
    }
    if ( accessor.max.empty() ) { max = std::vector<T>( nbComponents, defaultMax ); }
    else {
        std::transform( accessor.max.begin(),
                        accessor.max.end(),
                        std::back_inserter( max ),
                        []( float i ) { return static_cast<T>( i ); } );
    }
    int k = 0;
    for ( uint32_t i = 0; i < accessor.count; ++i ) {
        for ( int j = 0; j < nbComponents; ++j ) {
            if ( convertedData[k] > max[j] ) { convertedData[k] = max[j]; }
            else if ( convertedData[k] < min[j] ) { convertedData[k] = min[j]; }
            ++k;
        }
    }
}

template <typename T>
void sparseData( uint8_t* data,
                 const gltf::Accessor::Sparse& sparse,
                 int nbBytesByComponents,
                 const uint8_t* indices,
                 const uint8_t* values ) {
    auto indicesT = reinterpret_cast<const T*>( indices );
    for ( int i = 0; i < sparse.count; ++i ) {
        for ( int j = 0; j < nbBytesByComponents; ++j ) {
            data[indicesT[i] * nbBytesByComponents + j] = values[i * nbBytesByComponents + j];
        }
    }
}

template <typename T>
uint8_t* normalizeData( uint8_t* data, uint32_t nbComponents ) {
    T max           = std::numeric_limits<T>::max();
    auto dataT      = reinterpret_cast<T*>( data );
    auto* dataFloat = new float[nbComponents];
    for ( uint32_t i = 0; i < nbComponents; ++i ) {
        dataFloat[i] = std::max( dataT[i] / (float)max, -1.0f );
    }
    delete data;
    return (uint8_t*)dataFloat;
}

uint8_t* readBufferView( const gltf::Document& doc,
                         int bufferViewIndex,
                         int byteOffset,
                         int count,
                         int nbValueByComponents,
                         int nbByteByValue ) {
    const auto& bufferView  = doc.bufferViews[bufferViewIndex];
    const auto& buffer      = doc.buffers[bufferView.buffer];
    auto byteStride         = bufferView.byteStride;
    auto rawBuffer          = buffer.data.data();
    int nbBytesByComponents = nbValueByComponents * nbByteByValue;
    byteStride              = ( byteStride == 0 ) ? nbBytesByComponents : byteStride;
    auto data               = new uint8_t[count * nbBytesByComponents];
    rawBuffer               = rawBuffer + byteOffset + bufferView.byteOffset;

    // the data is store in little endian
    for ( int i = 0; i < count; ++i ) {
        for ( int j = 0; j < nbValueByComponents; ++j ) {
            for ( int k = 0; k < nbByteByValue; ++k ) {
                data[i * nbBytesByComponents + j * nbByteByValue + k] =
                    rawBuffer[j * nbByteByValue + k];
            }
        }
        rawBuffer = rawBuffer + byteStride;
    }

    return data;
}

void sparseCapDataNormalize( uint8_t*& data, const gltf::Document& doc, int accessorsIndex ) {
    gltf::Accessor accessor       = doc.accessors[accessorsIndex];
    gltf::Accessor::Sparse sparse = accessor.sparse;
    int nbValueByComponents       = nbValueByComponentMap.at( accessor.type );
    int nbByteByValue             = nbByteByValueMap.at( accessor.componentType );
    if ( !sparse.empty() ) {
        uint8_t* indices = readBufferView( doc,
                                           sparse.indices.bufferView,
                                           sparse.indices.byteOffset,
                                           sparse.count,
                                           1,
                                           nbByteByValueMap.at( sparse.indices.componentType ) );
        uint8_t* values  = readBufferView( doc,
                                          sparse.values.bufferView,
                                          sparse.values.byteOffset,
                                          sparse.count,
                                          nbValueByComponents,
                                          nbByteByValue );
        switch ( sparse.indices.componentType ) {
        case gltf::Accessor::ComponentType::UnsignedByte:
            sparseData<uint8_t>(
                data, sparse, nbValueByComponents * nbByteByValue, indices, values );
            break;
        case gltf::Accessor::ComponentType::UnsignedShort:
            sparseData<uint16_t>(
                data, sparse, nbValueByComponents * nbByteByValue, indices, values );
            break;
        case gltf::Accessor::ComponentType::UnsignedInt:
            sparseData<uint32_t>(
                data, sparse, nbValueByComponents * nbByteByValue, indices, values );
            break;
        default:
            LOG( logINFO ) << "Illegal type : sparse.indices.componentType";
            exit( 1 );
        }
        delete indices;
        delete values;
    }
    bool normalized = accessor.normalized;
    switch ( accessor.componentType ) {
    case gltf::Accessor::ComponentType::Byte:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<int8_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    case gltf::Accessor::ComponentType::UnsignedByte:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<uint8_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    case gltf::Accessor::ComponentType::Short:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<int16_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    case gltf::Accessor::ComponentType::UnsignedShort:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<uint16_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    case gltf::Accessor::ComponentType::UnsignedInt:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<uint32_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    case gltf::Accessor::ComponentType::Float:
        if ( !accessor.min.empty() || !accessor.max.empty() )
            minmax<float_t>( data, accessor, nbValueByComponents );
        if ( normalized )
            data = normalizeData<int8_t>( data, accessor.count * nbValueByComponents );
        break;
    default:
        LOG( logINFO ) << "Illegal type : accessor.componentType";
        exit( 1 );
    }
}

// Read the accessor and return a pointer where the data has been stored.
// The pointer can be cast to the accessor's type. If normalizes
uint8_t* AccessorReader::read( int32_t accessorIndex ) {
    // if data already in the map
    if ( m_accessors.find( accessorIndex ) != m_accessors.end() ) {
        return m_accessors[accessorIndex];
    }
    const gltf::Document& doc = m_doc;
    if ( 0 <= accessorIndex && accessorIndex < int32_t( doc.accessors.size() ) ) {
        gltf::Accessor accessor = doc.accessors[accessorIndex];
        int nbByteByValue       = nbByteByValueMap.at( accessor.componentType );
        int nbValueByComponent  = nbValueByComponentMap.at( accessor.type );
        uint8_t* data           = readBufferView( doc,
                                        accessor.bufferView,
                                        accessor.byteOffset,
                                        accessor.count,
                                        nbValueByComponent,
                                        nbByteByValue );
        // sparse and min-max
        sparseCapDataNormalize( data, doc, accessorIndex );
        // add data to map
        m_accessors.insert( std::pair<int32_t, uint8_t*>( accessorIndex, data ) );
        return data;
    }
    return nullptr;
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
