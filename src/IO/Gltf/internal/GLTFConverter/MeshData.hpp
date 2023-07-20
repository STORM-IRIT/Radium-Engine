#pragma once
#include <Core/Containers/VectorArray.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Log.hpp>
#include <IO/Gltf/internal/GLTFConverter/MaterialConverter.hpp>

#include <IO/Gltf/internal/fx/gltf.h>

#include <set>

namespace Ra::Core::Asset {
class GeometryData;
} // namespace Ra::Core::Asset

namespace Ra {
namespace IO {
namespace GLTF {

/**
 * Representation of geometrical data extracted from a json GLTF file
 */
class MeshData
{
  public:
    /**
     * Buffer information about the geometric data
     */
    struct BufferInfo {
        /**
         * The GLTF accesssor for the geometry component
         */
        fx::gltf::Accessor const* Accessor { nullptr };

        /**
         * the raw data representing the geometry component
         */
        uint8_t const* Data { nullptr };
        uint32_t DataStride { 0 };
        uint32_t TotalSize { 0 };

        /**
         * Test if the Mesh contains data
         * @return
         */
        [[nodiscard]] bool HasData() const noexcept { return Data != nullptr; }
    };

    /**
     * Constructor from a json document
     * @param doc the json document
     * @param meshIndex the index of the mesh in the json document
     * @param primitveIndex The index of the primitive description for the mesh
     */
    MeshData( const fx::gltf::Document& doc, int32_t meshIndex, int32_t primitveIndex ) :
        m_mode { doc.meshes[meshIndex].primitives[primitveIndex].mode } {
        const fx::gltf::Mesh& mesh           = doc.meshes[meshIndex];
        const fx::gltf::Primitive& primitive = mesh.primitives[primitveIndex];

        for ( const auto& attrib : primitive.attributes ) {
            if ( attrib.first == "POSITION" ) {
                m_vertexBuffer = GetData( doc, doc.accessors[attrib.second] );
            }
            else if ( attrib.first == "NORMAL" ) {
                m_normalBuffer = GetData( doc, doc.accessors[attrib.second] );
            }
            else if ( attrib.first == "TANGENT" ) {
                m_tangentBuffer = GetData( doc, doc.accessors[attrib.second] );
            }
            else if ( attrib.first == "TEXCOORD_0" ) {
                m_texCoord0Buffer = GetData( doc, doc.accessors[attrib.second] );
            }
        }

        if ( primitive.indices >= 0 ) {
            m_indexBuffer = GetData( doc, doc.accessors[primitive.indices] );
        }

        if ( primitive.material >= 0 ) {
            m_materialData.SetData( doc.materials[primitive.material] );
        }
    }

    /**
     *
     * @return the face buffer of the mesh
     */
    [[nodiscard]] const BufferInfo& IndexBuffer() const noexcept { return m_indexBuffer; }

    /**
     *
     * @return the vertex buffer of the mesh
     */
    [[nodiscard]] const BufferInfo& VertexBuffer() const noexcept { return m_vertexBuffer; }

    /**
     *
     * @return the normal buffer of the mesh
     */
    [[nodiscard]] const BufferInfo& NormalBuffer() const noexcept { return m_normalBuffer; }

    /**
     *
     * @return the tangent buffer of the mesh
     */
    [[nodiscard]] const BufferInfo& TangentBuffer() const noexcept { return m_tangentBuffer; }

    /**
     *
     * @return the texcoord buffer of the mesh
     */
    [[nodiscard]] const BufferInfo& TexCoord0Buffer() const noexcept { return m_texCoord0Buffer; }

    /**
     *
     * @return the mesh material data
     */
    [[nodiscard]] const MaterialData& Material() const noexcept { return m_materialData; }

    /**
     *
     * @return the primitive type (triangle, points, ...) of the mesh
     */
    fx::gltf::Primitive::Mode mode() { return m_mode; }

    /**
     *
     * @param doc the gltf document to read from
     * @param accessor
     * @return a vector of Vector4 joint indices
     */
    static void GetJoints( Ra::Core::VectorArray<Ra::Core::Vector4ui>& joints,
                           const fx::gltf::Document& doc,
                           const fx::gltf::Accessor& accessor ) {
        const auto buf = MeshData::GetData( doc, accessor );
        if ( buf.HasData() ) {
            if ( buf.Accessor->type != fx::gltf::Accessor::Type::Vec4 ) {
                LOG( Ra::Core::Utils::logERROR )
                    << "GLTF GetJoints -- Joint indices (JOINTS_*) must be Vec4 !"
                    << static_cast<uint8_t>( buf.Accessor->type );
            }
            else {
                switch ( buf.Accessor->componentType ) {
                case fx::gltf::Accessor::ComponentType::UnsignedByte: {
                    auto mem = buf.Data;
                    for ( uint32_t i = 0; i < buf.Accessor->count; ++i ) {
                        joints.push_back( Ra::Core::Vector4ui {
                            mem[4 * i], mem[4 * i + 1], mem[4 * i + 2], mem[4 * i + 3] } );
                    }
                    break;
                }
                case fx::gltf::Accessor::ComponentType::UnsignedShort: {
                    auto mem = reinterpret_cast<const unsigned short*>( buf.Data );
                    for ( uint32_t i = 0; i < buf.Accessor->count; ++i ) {
                        joints.push_back( Ra::Core::Vector4ui {
                            mem[4 * i], mem[4 * i + 1], mem[4 * i + 2], mem[4 * i + 3] } );
                    }
                    break;
                }
                default:
                    LOG( Ra::Core::Utils::logERROR )
                        << "GLTF GetJoints -- Joint indices (JOINTS_*) must be "
                        << "UnsignedByte, UnsignedShort or UnsignedInt !";
                }
            }
        }
    }

    /**
     *
     * @param doc the gltf document to read from
     * @param accessor
     * @return a vector of Vector4 weights
     */
    static void GetWeights( Ra::Core::VectorArray<Ra::Core::Vector4f>& weights,
                            const fx::gltf::Document& doc,
                            const fx::gltf::Accessor& accessor ) {
        const auto buf = MeshData::GetData( doc, accessor );
        if ( buf.HasData() ) {
            if ( buf.Accessor->type != fx::gltf::Accessor::Type::Vec4 ) {
                LOG( Ra::Core::Utils::logERROR ) << "GLTF GetWeights -- Weights must be Vec4 !"
                                                 << static_cast<uint8_t>( buf.Accessor->type );
            }
            else {
                switch ( buf.Accessor->componentType ) {
                case fx::gltf::Accessor::ComponentType::Float: {
                    const auto mem =
                        reinterpret_cast<const float*>( reinterpret_cast<const void*>( buf.Data ) );
                    for ( uint32_t i = 0; i < buf.Accessor->count; ++i ) {
                        weights.push_back( Ra::Core::Vector4f {
                            mem[4 * i], mem[4 * i + 1], mem[4 * i + 2], mem[4 * i + 3] } );
                    }
                    break;
                }
                case fx::gltf::Accessor::ComponentType::UnsignedByte: {
                    const auto mem = buf.Data;
                    for ( uint32_t i = 0; i < buf.Accessor->count; ++i ) {
                        weights.push_back(
                            Ra::Core::Vector4f { float( mem[4 * i] ) / UCHAR_MAX,
                                                 float( mem[4 * i + 1] ) / UCHAR_MAX,
                                                 float( mem[4 * i + 2] ) / UCHAR_MAX,
                                                 float( mem[4 * i + 3] ) / UCHAR_MAX } );
                    }
                    break;
                }
                case fx::gltf::Accessor::ComponentType::UnsignedShort: {
                    const auto mem = reinterpret_cast<const unsigned short*>( buf.Data );
                    for ( uint32_t i = 0; i < buf.Accessor->count; ++i ) {
                        weights.push_back(
                            Ra::Core::Vector4f { float( mem[4 * i] ) / USHRT_MAX,
                                                 float( mem[4 * i + 1] ) / USHRT_MAX,
                                                 float( mem[4 * i + 2] ) / USHRT_MAX,
                                                 float( mem[4 * i + 3] ) / USHRT_MAX } );
                    }
                    break;
                }
                default:
                    LOG( Ra::Core::Utils::logERROR )
                        << "GLTF GetWeights -- Weights must be Float or UnsignedByte or"
                        << " UnsignedShort !";
                }
            }
        }
    }

  private:
    BufferInfo m_indexBuffer {};
    BufferInfo m_vertexBuffer {};
    BufferInfo m_normalBuffer {};
    BufferInfo m_tangentBuffer {};
    BufferInfo m_texCoord0Buffer {};

    MaterialData m_materialData {};

    fx::gltf::Primitive::Mode m_mode;

    static BufferInfo GetData( const fx::gltf::Document& doc, const fx::gltf::Accessor& accessor ) {
        const fx::gltf::BufferView& bufferView = doc.bufferViews[accessor.bufferView];
        const fx::gltf::Buffer& buffer         = doc.buffers[bufferView.buffer];

        const uint32_t dataTypeSize = CalculateDataTypeSize( accessor );
        return BufferInfo { &accessor,
                            &buffer.data[bufferView.byteOffset + accessor.byteOffset],
                            dataTypeSize,
                            accessor.count * dataTypeSize };
    }

    static uint32_t CalculateDataTypeSize( const fx::gltf::Accessor& accessor ) noexcept {
        uint32_t elementSize;
        switch ( accessor.componentType ) {
        case fx::gltf::Accessor::ComponentType::Byte:
        case fx::gltf::Accessor::ComponentType::UnsignedByte:
            elementSize = 1;
            break;
        case fx::gltf::Accessor::ComponentType::Short:
        case fx::gltf::Accessor::ComponentType::UnsignedShort:
            elementSize = 2;
            break;
        case fx::gltf::Accessor::ComponentType::Float:
        case fx::gltf::Accessor::ComponentType::UnsignedInt:
            elementSize = 4;
            break;
        default:
            elementSize = 0;
        }

        switch ( accessor.type ) {
        case fx::gltf::Accessor::Type::Mat2:
            return 4 * elementSize;
        case fx::gltf::Accessor::Type::Mat3:
            return 9 * elementSize;
        case fx::gltf::Accessor::Type::Mat4:
            return 16 * elementSize;
        case fx::gltf::Accessor::Type::Scalar:
            return elementSize;
        case fx::gltf::Accessor::Type::Vec2:
            return 2 * elementSize;
        case fx::gltf::Accessor::Type::Vec3:
            return 3 * elementSize;
        case fx::gltf::Accessor::Type::Vec4:
            return 4 * elementSize;
        default:
            return 0;
        }
    }
};

class MeshNameCache
{
  public:
    static void resetCache();
    static size_t cacheSize();
    static std::pair<std::set<std::string>::iterator, bool> addName( const std::string& name );

  private:
    /** Collection of loaded names */
    static std::set<std::string> s_nameCache;
};

std::vector<std::unique_ptr<Ra::Core::Asset::GeometryData>>
buildMesh( const fx::gltf::Document& doc,
           int32_t meshIndex,
           const std::string& filePath,
           int32_t nodeNum );

} // namespace GLTF
} // namespace IO
} // namespace Ra
