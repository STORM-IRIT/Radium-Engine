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
template <typename T>
void convertVectors( Vector3Array& vectors, const void* data, int count ) {
    auto mem = reinterpret_cast<const T*>( data );
    for ( int i = 0; i < count; ++i ) {
        vectors.emplace_back( mem[3 * i], mem[3 * i + 1], mem[3 * i + 2] );
    }
}

// GLTF texCoord are vec2
// Warning : textCoord could be normalized integers
template <typename T>
void convertTexCoord( Vector3Array& vectors, const void* data, int count ) {
    auto mem = reinterpret_cast<const T*>( data );
    for ( int i = 0; i < count; ++i ) {
        float u = float( mem[2 * i] ) / std::numeric_limits<T>::max();
        float v = 1 - float( mem[2 * i + 1] ) / std::numeric_limits<T>::max();
        vectors.emplace_back( u, v, 0 );
    }
}

template <>
void convertTexCoord<float>( Vector3Array& vectors, const void* data, int count ) {
    auto mem = reinterpret_cast<const float*>( data );
    for ( int i = 0; i < count; ++i ) {
        vectors.emplace_back( mem[2 * i], 1 - mem[2 * i + 1], 0 );
    }
}

// GLTF tangents are vec4 with the last component indicating handedness.
// Multiply the tangent coordinates by the handedness to have always right handed local frame
// Must verify this
template <typename T>
void convertTangents( Vector3Array& vectors, const void* data, int count ) {
    auto mem = reinterpret_cast<const T*>( data );
    for ( int i = 0; i < count; ++i ) {
        vectors.emplace_back( mem[4 * i] * mem[4 * i + 3],
                              mem[4 * i + 1] * mem[4 * i + 3],
                              mem[4 * i + 2] * mem[4 * i + 3] );
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
        const MeshData::BufferInfo& vBuffer = mesh.VertexBuffer();
        const MeshData::BufferInfo& nBuffer = mesh.NormalBuffer();
        const MeshData::BufferInfo& tBuffer = mesh.TangentBuffer();
        const MeshData::BufferInfo& cBuffer = mesh.TexCoord0Buffer();
        const MeshData::BufferInfo& iBuffer = mesh.IndexBuffer();

        // we need at least vertices to render an object
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
            convertVectors<float>( vertices, vBuffer.Data, vBuffer.Accessor->count );
            meshPart->getGeometry().vertexAttribs().unlock( attribVertices );
            // Convert faces
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
                convertVectors<float>( normals, nBuffer.Data, nBuffer.Accessor->count );
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else {
                NormalCalculator nrmCalculator;
                nrmCalculator( meshPart.get() );
            }
            // Convert TexCoord if any
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
                        texcoords, cBuffer.Data, cBuffer.Accessor->count );
                    break;
                case gltf::Accessor::ComponentType::UnsignedShort:
                    convertTexCoord<unsigned short>(
                        texcoords, cBuffer.Data, cBuffer.Accessor->count );
                    break;
                case gltf::Accessor::ComponentType::Float:
                    convertTexCoord<float>( texcoords, cBuffer.Data, cBuffer.Accessor->count );
                    break;
                default:
                    LOG( logERROR ) << "GLTF buildMesh -- texCoord must be UnsignedByte, "
                                       "UnsignedShort or Float !";
                    continue;
                }
                meshPart->getGeometry().vertexAttribs().unlock( attribHandle );
            }
            else { LOG( logDEBUG ) << "GLTF buildMesh -- No texCoord provided. !"; }
            // Convert tangent if any
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
                convertTangents<float>( tangents, tBuffer.Data, tBuffer.Accessor->count );
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
