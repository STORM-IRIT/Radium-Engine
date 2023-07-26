#include <IO/Gltf/internal/GLTFConverter/MeshData.hpp>
#include <IO/Gltf/internal/GLTFConverter/NormalCalculator.hpp>
#include <IO/Gltf/internal/GLTFConverter/TangentCalculator.hpp>

using namespace fx;

namespace Ra {
namespace IO {
namespace GLTF {

using namespace Ra::Core;
using namespace Ra::Core::Asset;
using namespace Ra::Core::Utils;

// used to convert position and normals and ...
// takes care of interleaved buffers
template <typename T>
void convertVectors( Vector3Array& vectors,
                     const uint8_t* data,
                     uint32_t count,
                     uint32_t stride = 0 ) {
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawVector = reinterpret_cast<const T*>( data );
        vectors.emplace_back( rawVector[0], rawVector[1], rawVector[2] );
        data += std::max( uint32_t( 3 * sizeof( T ) ), stride );
    }
}

// GLTF texCoord are vec2
// takes care of interleaved buffers
// Warning : textCoord could be normalized integers
template <typename T>
void convertTexCoord( Vector3Array& vectors,
                      const uint8_t* data,
                      uint32_t count,
                      uint32_t stride = 0 ) {
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawTexCoord = reinterpret_cast<const T*>( data );
        float u          = float( rawTexCoord[0] ) / std::numeric_limits<T>::max();
        float v          = 1 - float( rawTexCoord[1] ) / std::numeric_limits<T>::max();
        vectors.emplace_back( u, v, 0 );
        data += std::max( uint32_t( 2 * sizeof( T ) ), stride );
    }
}

template <>
void convertTexCoord<float>( Vector3Array& vectors,
                             const uint8_t* data,
                             uint32_t count,
                             uint32_t stride ) {
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawVector = reinterpret_cast<const float*>( data );
        vectors.emplace_back( rawVector[0], 1 - rawVector[1], 0 );
        data += std::max( uint32_t( 2 * sizeof( float ) ), stride );
    }
}

// GLTF colors are vec3 or vec4. if vec3, alpha is assumed to be 1
// takes care of interleaved buffers
// Warning : Colors could be normalized integers
template <typename T>
void convertColor( Vector4Array& colors,
                   const uint8_t* data,
                   fx::gltf::Accessor::Type type,
                   uint32_t count,
                   uint32_t stride = 0 ) {
    uint32_t numComponents = 3;
    if ( type == gltf::Accessor::Type::Vec4 ) { numComponents = 4; }
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawColors = reinterpret_cast<const T*>( data );
        Vector4 clr { 0, 0, 0, 1 };
        for ( uint32_t c = 0; c < numComponents; c++ ) {
            clr[c] = float( rawColors[c] ) / std::numeric_limits<T>::max();
        }
        colors.emplace_back( clr );
        data += std::max( uint32_t( numComponents * sizeof( T ) ), stride );
    }
}

template <>
void convertColor<float>( Vector4Array& colors,
                          const uint8_t* data,
                          fx::gltf::Accessor::Type type,
                          uint32_t count,
                          uint32_t stride ) {
    uint32_t numComponents = 3;
    if ( type == gltf::Accessor::Type::Vec4 ) { numComponents = 4; }
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawColors = reinterpret_cast<const float*>( data );
        Vector4 clr { 0, 0, 0, 1 };
        for ( uint32_t c = 0; c < numComponents; c++ ) {
            clr[c] = rawColors[c];
        }
        colors.emplace_back( clr );
        data += std::max( uint32_t( numComponents * sizeof( float ) ), stride );
    }
}

// GLTF tangents are vec4 with the last component indicating handedness.
// Multiply the tangent coordinates by the handedness to have always right handed local frame
// Must verify this
template <typename T>
void convertTangents( Vector3Array& vectors,
                      const uint8_t* data,
                      uint32_t count,
                      uint32_t stride = 0 ) {
    for ( uint32_t i = 0; i < count; ++i ) {
        auto rawVector = reinterpret_cast<const T*>( data );
        vectors.emplace_back(
            rawVector[0] * rawVector[3], rawVector[1] * rawVector[3], rawVector[2] * rawVector[3] );
        data += std::max( uint32_t( 4 * sizeof( T ) ), stride );
    }
}

// used to convert face indices
template <typename T>
void convertIndices( Vector3uArray& indices, const uint8_t* data, uint32_t count ) {
    auto mem = reinterpret_cast<const T*>( data );
    for ( uint32_t i = 0; i < count; ++i ) {
        indices.push_back( { mem[3 * i], mem[3 * i + 1], mem[3 * i + 2] } );
    }
}

std::vector<std::unique_ptr<Ra::Core::Asset::GeometryData>> buildMesh( const gltf::Document& doc,
                                                                       int32_t meshIndex,
                                                                       const std::string& filePath,
                                                                       int32_t nodeNum ) {
    std::vector<std::unique_ptr<Ra::Core::Asset::GeometryData>> meshParts;
    for ( int32_t meshPartNumber = 0;
          meshPartNumber < int32_t( doc.meshes[meshIndex].primitives.size() );
          ++meshPartNumber ) {
        MeshData mesh { doc, meshIndex, meshPartNumber };
        if ( mesh.mode() != fx::gltf::Primitive::Mode::Triangles ) {
            LOG( logERROR )
                << "GLTF buildMesh -- RadiumGLTF only supports Triangles primitive right now !";
            continue;
        }

        // we need at least vertices to render an object
        const MeshData::BufferInfo& vBuffer = mesh.VertexBuffer();
        if ( vBuffer.HasData() ) {
            std::string meshName = doc.meshes[meshIndex].name;
            if ( meshName.empty() ) {
                meshName = "mesh_";
                meshName += "_n_" + std::to_string( nodeNum ) + "_m_" +
                            std::to_string( meshIndex ) + "_p_" + std::to_string( meshPartNumber );
            }
            auto nameIsNew = MeshNameCache::addName( meshName );
            if ( !nameIsNew.second ) {
                meshName += "_" + std::to_string( meshIndex );
                MeshNameCache::addName( meshName );
            }

            auto meshPart =
                std::make_unique<GeometryData>( meshName, GeometryData::GeometryType::TRI_MESH );
            // Convert vertices
            if ( ( vBuffer.Accessor->type != gltf::Accessor::Type::Vec3 ) ||
                 ( vBuffer.Accessor->componentType != gltf::Accessor::ComponentType::Float ) ) {
                LOG( logERROR ) << "GLTF buildMesh -- Vertices must be Vec3 of Float!";
                continue;
            }
            auto attribVertices = meshPart->getGeometry().addAttrib<Vector3>(
                getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_POSITION ) );
            auto& vertices =
                meshPart->getGeometry().vertexAttribs().getDataWithLock( attribVertices );
            vertices.reserve( vBuffer.Accessor->count );
            convertVectors<float>(
                vertices, vBuffer.Data, vBuffer.Accessor->count, vBuffer.DataStride );
            meshPart->getGeometry().vertexAttribs().unlock( attribVertices );

            // Convert faces
            const MeshData::BufferInfo& iBuffer = mesh.IndexBuffer();
            if ( iBuffer.HasData() ) {
                if ( iBuffer.Accessor->type != gltf::Accessor::Type::Scalar ) {
                    if ( iBuffer.Accessor->type == gltf::Accessor::Type::None ) {
                        LOG( logERROR ) << "GLTF buildMesh -- Indices must be Scalar !"
                                        << static_cast<uint8_t>( iBuffer.Accessor->type );
                        continue;
                    }
                }
                else {
                    // Gltf only support triangle mesh right now
                    meshPart->setPrimitiveCount( iBuffer.Accessor->count / 3 );
                    auto layer    = std::make_unique<Ra::Core::Geometry::TriangleIndexLayer>();
                    auto& indices = layer->collection();
                    indices.reserve( meshPart->getPrimitiveCount() );
                    switch ( iBuffer.Accessor->componentType ) {
                    case gltf::Accessor::ComponentType::UnsignedByte:
                        convertIndices<unsigned char>(
                            indices, iBuffer.Data, meshPart->getPrimitiveCount() );
                        break;
                    case gltf::Accessor::ComponentType::UnsignedShort:
                        convertIndices<unsigned short>(
                            indices, iBuffer.Data, meshPart->getPrimitiveCount() );
                        break;
                    case gltf::Accessor::ComponentType::UnsignedInt:
                        convertIndices<unsigned int>(
                            indices, iBuffer.Data, meshPart->getPrimitiveCount() );
                        break;
                    default:
                        LOG( logERROR ) << "GLTF buildMesh -- Indices must be UnsignedByte, "
                                           "UnsignedShort or UnsignedInt !";
                        continue;
                    }
                    meshPart->getGeometry().addLayer( std::move( layer ), false, "indices" );
                }
            }
            else {
                meshPart->setPrimitiveCount( meshPart->getGeometry().vertices().size() / 3 );
                auto layer    = std::make_unique<Ra::Core::Geometry::TriangleIndexLayer>();
                auto& indices = layer->collection();
                indices.reserve( meshPart->getPrimitiveCount() );
                for ( uint vi = 0; vi < uint( meshPart->getPrimitiveCount() ); ++vi ) {
                    indices.emplace_back( Vector3ui { 3 * vi, 3 * vi + 1, 3 * vi + 2 } );
                }
                meshPart->getGeometry().addLayer( std::move( layer ), false, "indices" );
            }

            // Convert or compute normals
            const MeshData::BufferInfo& nBuffer = mesh.NormalBuffer();
            if ( nBuffer.HasData() ) {
                if ( ( nBuffer.Accessor->type != gltf::Accessor::Type::Vec3 ) ||
                     ( nBuffer.Accessor->componentType != gltf::Accessor::ComponentType::Float ) ) {
                    LOG( logERROR ) << "GLTF buildMesh -- Normals must be Vec3 of Float!";
                    continue;
                }
                auto attribHandle = meshPart->getGeometry().addAttrib<Vector3>(
                    getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_NORMAL ) );
                auto& normals =
                    meshPart->getGeometry().vertexAttribs().getDataWithLock( attribHandle );
                normals.reserve( nBuffer.Accessor->count );
                convertVectors<float>(
                    normals, nBuffer.Data, nBuffer.Accessor->count, nBuffer.DataStride );
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else {
                NormalCalculator nrmCalculator;
                nrmCalculator( meshPart.get() );
            }

            // Convert TexCoord if any
            // As Radium only manage 1 texture coordinate set, only use the texcoord0
            const MeshData::BufferInfo& cBuffer = mesh.TexCoordBuffer( 0 );
            if ( cBuffer.HasData() ) {
                if ( ( cBuffer.Accessor->type != gltf::Accessor::Type::Vec2 ) ) {
                    LOG( logERROR ) << "GLTF buildMesh -- TexCoord must be Vec2";
                    continue;
                }
                auto attribHandle = meshPart->getGeometry().addAttrib<Vector3>(
                    getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TEXCOORD ) );
                auto& texcoords =
                    meshPart->getGeometry().vertexAttribs().getDataWithLock( attribHandle );
                texcoords.reserve( cBuffer.Accessor->count );
                switch ( cBuffer.Accessor->componentType ) {
                case gltf::Accessor::ComponentType::UnsignedByte:
                    convertTexCoord<unsigned char>(
                        texcoords, cBuffer.Data, cBuffer.Accessor->count, cBuffer.DataStride );
                    break;
                case gltf::Accessor::ComponentType::UnsignedShort:
                    convertTexCoord<unsigned short>(
                        texcoords, cBuffer.Data, cBuffer.Accessor->count, cBuffer.DataStride );
                    break;
                case gltf::Accessor::ComponentType::Float:
                    convertTexCoord<float>(
                        texcoords, cBuffer.Data, cBuffer.Accessor->count, cBuffer.DataStride );
                    break;
                default:
                    LOG( logERROR ) << "GLTF buildMesh -- texCoord must be UnsignedByte, "
                                       "UnsignedShort or Float !";
                    continue;
                }
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else { LOG( logDEBUG ) << "GLTF buildMesh -- No texCoord provided. !"; }

            // Convert vertexColorColor if any (limited to one COLOR attribute)
            const MeshData::BufferInfo& colorBuffer = mesh.ColorBuffer();
            if ( colorBuffer.HasData() ) {
                if ( ( colorBuffer.Accessor->type != gltf::Accessor::Type::Vec3 ) &&
                     ( colorBuffer.Accessor->type != gltf::Accessor::Type::Vec4 ) ) {
                    LOG( logERROR ) << "GLTF buildMesh -- Color must be Vec3 or Vec4";
                    continue;
                }
                // Radium colors are always vec4
                auto attribHandle = meshPart->getGeometry().addAttrib<Vector4>(
                    getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_COLOR ) );
                auto& colorAtttrib =
                    meshPart->getGeometry().vertexAttribs().getDataWithLock( attribHandle );
                colorAtttrib.reserve( colorBuffer.Accessor->count );
                switch ( colorBuffer.Accessor->componentType ) {
                case gltf::Accessor::ComponentType::UnsignedByte:
                    convertColor<unsigned char>( colorAtttrib,
                                                 colorBuffer.Data,
                                                 colorBuffer.Accessor->type,
                                                 colorBuffer.Accessor->count,
                                                 colorBuffer.DataStride );
                    break;
                case gltf::Accessor::ComponentType::UnsignedShort:
                    convertColor<unsigned short>( colorAtttrib,
                                                  colorBuffer.Data,
                                                  colorBuffer.Accessor->type,
                                                  colorBuffer.Accessor->count,
                                                  colorBuffer.DataStride );
                    break;
                case gltf::Accessor::ComponentType::Float:
                    convertColor<float>( colorAtttrib,
                                         colorBuffer.Data,
                                         colorBuffer.Accessor->type,
                                         colorBuffer.Accessor->count,
                                         colorBuffer.DataStride );
                    break;
                default:
                    LOG( logERROR ) << "GLTF buildMesh -- Color attrib must be UnsignedByte, "
                                       "UnsignedShort or Float !";
                    continue;
                }
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else { LOG( logDEBUG ) << "GLTF buildMesh -- No Color attrib provided. !"; }

            // Convert tangent if any
            const MeshData::BufferInfo& tBuffer = mesh.TangentBuffer();
            if ( tBuffer.HasData() ) {
                if ( ( tBuffer.Accessor->type != gltf::Accessor::Type::Vec4 ) ||
                     ( tBuffer.Accessor->componentType != gltf::Accessor::ComponentType::Float ) ) {
                    LOG( logERROR ) << "GLTF buildMesh -- Tangents must be Vec4 of Float!";
                    continue;
                }
                auto attribHandle = meshPart->getGeometry().addAttrib<Vector3>(
                    getAttribName( Ra::Core::Geometry::MeshAttrib::VERTEX_TANGENT ) );
                auto& tangents =
                    meshPart->getGeometry().vertexAttribs().getDataWithLock( attribHandle );
                tangents.reserve( tBuffer.Accessor->count );
                convertTangents<float>(
                    tangents, tBuffer.Data, tBuffer.Accessor->count, tBuffer.DataStride );
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else {
                if ( cBuffer.HasData() ) {
                    //                   LOG(logINFO) << "GLTF buildMesh -- No tangents provided.
                    //                   Must computed tangents!";
                    TangentCalculator tgtBuilder;
                    tgtBuilder( meshPart.get() );
                }
                else {
                    LOG( logDEBUG ) << "GLTF buildMesh -- No tangents nor texcoords. Texture "
                                       "mapping will be not correct!";
                }
            }
            // MATERIAL PART
            meshPart->setMaterial(
                buildMaterial( doc, meshIndex, filePath, meshPartNumber, mesh.Material() ) );

            meshParts.emplace_back( std::move( meshPart ) );
        }
        else { LOG( logERROR ) << "GLTF converter -- No vertices found, skip primitive."; }
    }
    return meshParts;
}

std::set<std::string> MeshNameCache::s_nameCache;

void MeshNameCache::resetCache() {
    s_nameCache.clear();
}

size_t MeshNameCache::cacheSize() {
    return s_nameCache.size();
}

std::pair<std::set<std::string>::iterator, bool> MeshNameCache::addName( const std::string& name ) {
    return s_nameCache.insert( name );
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
