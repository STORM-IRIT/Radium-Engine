#include <Core/Utils/Log.hpp>
#include <Engine/Data/GLTFMaterial.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/MetallicRoughnessMaterial.hpp>
#include <Engine/Data/SpecularGlossinessMaterial.hpp>
#include <Engine/RadiumEngine.hpp>
#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Scene/Component.hpp>
#include <Engine/Scene/Entity.hpp>
#include <IO/Gltf/Writer/glTFFileWriter.hpp>

#include <IO/Gltf/internal/Extensions/MaterialExtensions.hpp>
#include <IO/Gltf/internal/fx/gltf.h>
#include <memory>

/// @todo allow to export standard Radium Materials
#ifdef EXPORT_BLINNPHONG
// To export Radium materials :
#    include <Engine/Data/BlinnPhongMaterial.hpp>
#endif

using namespace fx;

namespace Ra {
namespace IO {
namespace GLTF {
using namespace Ra::Core::Utils;
using namespace Ra::Engine;

/// The used and required GLTF extension saved by the writer
static std::set<std::string> gltf_usedExtensions;
static std::set<std::string> gltf_requiredExtensions;
/// The texture uri prefix
static std::string g_texturePrefix { "textures/" };
/**
 * FileWriter for GLTF2.0 file format
 * TODO : a huge refactoring is expected so that a to_json method is defined on each element to
 * export in GLTF.
 *
 */
glTFFileWriter::glTFFileWriter( std::string filename,
                                std::string texturePrefix,
                                bool writeImages ) :
    m_fileName { std::move( filename ) },
    m_texturePrefix { std::move( texturePrefix ) },
    m_writeImages { writeImages } {
    LOG( logINFO ) << "GLTF2 Writer : saving to file " << m_fileName << " with texture prefix "
                   << m_texturePrefix;
    // Constructing root node name
    auto p = m_fileName.find_last_of( "/\\" );
    if ( p != std::string::npos ) { m_rootName = m_fileName.substr( p + 1 ); }
    else { m_rootName = m_fileName; }
    p = m_rootName.find_last_of( '.' );
    if ( p != std::string::npos ) { m_rootName = m_rootName.substr( 0, p ); }
    m_bufferName = m_rootName + ".bin";
}

glTFFileWriter::~glTFFileWriter() {}

void fillTransform( gltf::Node& node, const Ra::Core::Transform& transform ) {
    // Decompose the current transform into T*R*S
    Ra::Core::Matrix3 rotationMat;
    Ra::Core::Matrix3 scaleMat;
    Ra::Core::Vector3 translate = transform.translation();
    transform.computeRotationScaling( &rotationMat, &scaleMat );
    Ra::Core::Quaternion quat( rotationMat );
    node.rotation    = { quat.x(), quat.y(), quat.z(), quat.w() };
    node.translation = { translate.x(), translate.y(), translate.z() };
    node.scale       = { scaleMat( 0, 0 ), scaleMat( 1, 1 ), scaleMat( 2, 2 ) };
}

int addIndices( gltf::Document& document,
                int buffer,
                const Ra::Core::Geometry::TriangleMesh& geometry ) {
    gltf::Buffer& theBuffer = document.buffers[buffer];

    // 1 - Build a bufferview for the indices
    document.bufferViews.push_back( gltf::BufferView {} );
    gltf::BufferView& bufferView = document.bufferViews.back();
    bufferView.buffer            = buffer;
    bufferView.target            = gltf::BufferView::TargetType::ElementArrayBuffer;
    bufferView.byteOffset        = uint32_t( theBuffer.data.size() );
    bufferView.byteLength        = 3 * geometry.getIndices().size() * sizeof( unsigned int );

    // 2 - append indices to the binary buffer
    theBuffer.data.reserve( bufferView.byteOffset + bufferView.byteLength );
    theBuffer.data.resize( bufferView.byteOffset + bufferView.byteLength );
    std::memcpy( theBuffer.data.data() + bufferView.byteOffset,
                 reinterpret_cast<const uint8_t*>( geometry.getIndices().data() ),
                 bufferView.byteLength );
    theBuffer.byteLength = uint32_t( theBuffer.data.size() );

    // 3 - Build an accessor for the indices.
    document.accessors.push_back( gltf::Accessor {} );
    gltf::Accessor& accessor = document.accessors.back();
    // compute bounds on the indice values
    size_t ix_min = geometry.getIndices()[0]( 0 );
    size_t ix_max = ix_min;
    auto minmax   = [&ix_min, &ix_max]( const Ra::Core::Vector3ui& t ) {
        if ( t( 0 ) < ix_min ) { ix_min = t( 0 ); }
        else if ( t( 0 ) > ix_max ) { ix_max = t( 0 ); }

        if ( t( 1 ) < ix_min ) { ix_min = t( 1 ); }
        else if ( t( 1 ) > ix_max ) { ix_max = t( 1 ); }

        if ( t( 2 ) < ix_min ) { ix_min = t( 2 ); }
        else if ( t( 2 ) > ix_max ) { ix_max = t( 2 ); }
    };
    std::for_each( geometry.getIndices().cbegin(), geometry.getIndices().cend(), minmax );
    accessor.min.push_back( ix_min );
    accessor.max.push_back( ix_max );
    accessor.bufferView    = uint32_t( document.bufferViews.size() - 1 );
    accessor.byteOffset    = 0;
    accessor.count         = 3 * geometry.getIndices().size();
    accessor.componentType = gltf::Accessor::ComponentType::UnsignedInt;
    accessor.type          = gltf::Accessor::Type::Scalar;
    return document.accessors.size() - 1;
}

// Functor applied on mesh attributes to build gltf equivalent.
class VertexAttribWriter
{
  public:
    VertexAttribWriter( gltf::Document& document, int buffer, gltf::Primitive& primitive ) :
        m_document( document ), m_buffer( buffer ), m_primitive( primitive ) {}
    ~VertexAttribWriter() = default;
    void operator()( const Ra::Core::Utils::AttribBase* att ) const;

  private:
    gltf::Document& m_document;
    int m_buffer;
    gltf::Primitive& m_primitive;

    static std::map<std::string, std::string> translator;
};

std::map<std::string, std::string> VertexAttribWriter::translator {
    { "in_position", "POSITION" },
    { "in_normal", "NORMAL" },
    { "in_tangent", "TANGENT" },
    { "in_texcoord", "TEXCOORD_0" },
    { "in_texcoord_1", "TEXCOORD_1" },
    { "in_color", "COLOR_0" },
    { "in_joints", "JOINTS_0" },
    { "in_weights", "WEIGHTS_0" } };

void VertexAttribWriter::operator()( const Ra::Core::Utils::AttribBase* att ) const {
    auto name = translator.find( att->getName() );
    if ( name == translator.end() ) {
        LOG( logERROR ) << "Not exporting invalid vertex attribute for GLTF : " << att->getName();
        return;
    }

    gltf::Buffer& theBuffer = m_document.buffers[m_buffer];
    gltf::Accessor accessor;
    accessor.byteOffset    = 0;
    accessor.componentType = gltf::Accessor::ComponentType::Float;
    accessor.count         = att->getSize();

    // TODO : only 1 bufferview per type attrib type per RenderObject ...
    gltf::BufferView bufferView;
    bufferView.buffer     = m_buffer;
    bufferView.target     = gltf::BufferView::TargetType::ArrayBuffer;
    bufferView.byteOffset = theBuffer.data.size();

    // Fill the node or exit if gltf node is not valid
    // see https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#meshes
    const uint8_t* dataToCopy { reinterpret_cast<const uint8_t*>( att->dataPtr() ) };
    uint32_t dataByteLength { uint32_t( att->getBufferSize() ) };

    // One of these vector will be used to convert data type and will be delete automatically at the
    // end of the function
    std::vector<Ra::Core::Vector2> vec2ConvertedData;
    std::vector<Ra::Core::Vector3> vec3ConvertedData;
    std::vector<Ra::Core::Vector4> vec4ConvertedData;

    if ( ( name->second == "POSITION" ) || ( name->second == "NORMAL" ) ) {
        // only Vec3 is allowed for POSITION and NORMAL
        // Chances that Radium manage these attribs as Vec3
        if ( att->isVector4() ) {
            // Need to convert data :(
            vec3ConvertedData.reserve( att->getSize() );
            auto& s = att->cast<Ra::Core::Vector4>().data();
            std::transform( s.begin(),
                            s.end(),
                            std::back_inserter( vec3ConvertedData ),
                            []( const Ra::Core::Vector4& c ) {
                                return Ra::Core::Vector3 { c.x(), c.y(), c.z() };
                            } );
            dataToCopy     = reinterpret_cast<const uint8_t*>( vec3ConvertedData.data() );
            dataByteLength = vec3ConvertedData.size() * sizeof( Ra::Core::Vector3 );
        }
        else if ( !att->isVector3() ) {
            LOG( logERROR )
                << "POSITION and NORMAL vertex attributes must be Vec3 or Vec4 for gltf export of "
                << att->getName();
            return;
        }
        accessor.type = gltf::Accessor::Type::Vec3;
        // Compute min and max on the attribute value
        const Ra::Core::Vector3* arrayOfAttribs =
            reinterpret_cast<const Ra::Core::Vector3*>( dataToCopy );
        Ra::Core::Vector3 minAtt = arrayOfAttribs[0];
        Ra::Core::Vector3 maxAtt = arrayOfAttribs[0];
        for ( size_t i = 0; i < att->getSize(); ++i ) {
            if ( arrayOfAttribs[i].x() < minAtt.x() ) { minAtt.x() = arrayOfAttribs[i].x(); }
            else if ( arrayOfAttribs[i].x() > maxAtt.x() ) { maxAtt.x() = arrayOfAttribs[i].x(); }
            if ( arrayOfAttribs[i].y() < minAtt.y() ) { minAtt.y() = arrayOfAttribs[i].y(); }
            else if ( arrayOfAttribs[i].y() > maxAtt.y() ) { maxAtt.y() = arrayOfAttribs[i].y(); }
            if ( arrayOfAttribs[i].z() < minAtt.z() ) { minAtt.z() = arrayOfAttribs[i].z(); }
            else if ( arrayOfAttribs[i].z() > maxAtt.z() ) { maxAtt.z() = arrayOfAttribs[i].z(); }
        }
        accessor.min.push_back( minAtt.x() );
        accessor.min.push_back( minAtt.y() );
        accessor.min.push_back( minAtt.z() );
        accessor.max.push_back( maxAtt.x() );
        accessor.max.push_back( maxAtt.y() );
        accessor.max.push_back( maxAtt.z() );
    }
    else if ( ( name->second == "COLOR_0" ) ) {
        // only Vec3 or Vec4  is allowed for COLOR_0
        if ( att->isVector3() ) { accessor.type = gltf::Accessor::Type::Vec3; }
        else if ( att->isVector4() ) { accessor.type = gltf::Accessor::Type::Vec4; }
        else {
            LOG( logERROR ) << "COLOR_0 vertex attributes must be Vec3 or Vec4 for gltf export of "
                            << att->getName();
            return;
        }
    }
    else if ( ( name->second == "TANGENT" ) ) {
        // only Vec4 is allowed for Tangent
        if ( att->isVector3() ) {
            // Radium manage tangents as Vec3
            // Need to convert tangent :(
            vec4ConvertedData.reserve( att->getSize() );
            auto& s = att->cast<Ra::Core::Vector3>().data();
            // TODO : verify handedness here: 1_ra could be -1_ra ...
            std::transform( s.begin(),
                            s.end(),
                            std::back_inserter( vec4ConvertedData ),
                            []( const Ra::Core::Vector3& c ) {
                                return Ra::Core::Vector4 { c.x(), c.y(), c.z(), 1_ra };
                            } );
            dataToCopy     = reinterpret_cast<const uint8_t*>( vec4ConvertedData.data() );
            dataByteLength = vec4ConvertedData.size() * sizeof( Ra::Core::Vector4 );
        }
        else if ( !att->isVector4() ) {
            LOG( logERROR ) << "TANGENT vertex attributes must be Vec3 or Vec4 for gltf export of "
                            << att->getName();
            return;
        }
        accessor.type = gltf::Accessor::Type::Vec4;
    }
    else if ( ( name->second.substr( 0, 8 ) == "TEXCOORD" ) ) {
        // only Vec2 is allowed for TexCoord
        if ( att->isVector4() ) {
            // Radium manage TexCoord as Vec4
            // Need to convert TexCoord :(
            vec2ConvertedData.reserve( att->getSize() );
            auto& s = att->cast<Ra::Core::Vector4>().data();
            // Warning : here, 1-v is done according to GLTF specification for image reference (see
            // the way gltf file is loaded)
            std::transform( s.begin(),
                            s.end(),
                            std::back_inserter( vec2ConvertedData ),
                            []( const Ra::Core::Vector4& c ) {
                                return Ra::Core::Vector2 { c.x(), 1 - c.y() };
                            } );
            dataToCopy     = reinterpret_cast<const uint8_t*>( vec2ConvertedData.data() );
            dataByteLength = vec2ConvertedData.size() * sizeof( Ra::Core::Vector2 );
        }
        else if ( att->isVector3() ) {
            // Need to convert TexCoord :(
            vec2ConvertedData.reserve( att->getSize() );
            auto& s = att->cast<Ra::Core::Vector3>().data();
            // TODO : verify handedness here: 1_ra could be -1_ra ...
            std::transform( s.begin(),
                            s.end(),
                            std::back_inserter( vec2ConvertedData ),
                            []( const Ra::Core::Vector3& c ) {
                                return Ra::Core::Vector2 { c.x(), 1 - c.y() };
                            } );
            dataToCopy     = reinterpret_cast<const uint8_t*>( vec2ConvertedData.data() );
            dataByteLength = vec2ConvertedData.size() * sizeof( Ra::Core::Vector2 );
        }
        else if ( !att->isVector2() ) {
            LOG( logERROR ) << "TEXCOORD vertex attributes must be Vec2 (or vec3/vec4 converted) "
                               "for gltf export of "
                            << att->getName();
            return;
        }
        accessor.type = gltf::Accessor::Type::Vec2;
        // Compute min and max on the attribute value
        const Ra::Core::Vector2* arrayOfAttribs =
            reinterpret_cast<const Ra::Core::Vector2*>( dataToCopy );
        Ra::Core::Vector2 minAtt = arrayOfAttribs[0];
        Ra::Core::Vector2 maxAtt = arrayOfAttribs[0];
        for ( size_t i = 0; i < att->getSize(); ++i ) {
            if ( arrayOfAttribs[i].x() < minAtt.x() ) { minAtt.x() = arrayOfAttribs[i].x(); }
            else if ( arrayOfAttribs[i].x() > maxAtt.x() ) { maxAtt.x() = arrayOfAttribs[i].x(); }
            if ( arrayOfAttribs[i].y() < minAtt.y() ) { minAtt.y() = arrayOfAttribs[i].y(); }
            else if ( arrayOfAttribs[i].y() > maxAtt.y() ) { maxAtt.y() = arrayOfAttribs[i].y(); }
        }
        accessor.min.push_back( minAtt.x() );
        accessor.min.push_back( minAtt.y() );
        accessor.max.push_back( maxAtt.x() );
        accessor.max.push_back( maxAtt.y() );
    }
    else {
        /// TODO : implement theGLTF export of vertex attribs JOINTS_0 and WEIGHTS_0
        LOG( logWARNING ) << "Attribute " << name->second << " (from " << att->getName()
                          << ") is not yet expoerted.";
        return;
    }

    bufferView.byteLength = dataByteLength;
    theBuffer.data.resize( bufferView.byteOffset + bufferView.byteLength );
    std::memcpy( theBuffer.data.data() + bufferView.byteOffset, dataToCopy, dataByteLength );
    theBuffer.byteLength = theBuffer.data.size();

    // Add the bufferview
    m_document.bufferViews.push_back( bufferView );
    // update and add the accessor
    accessor.bufferView = m_document.bufferViews.size() - 1;
    m_document.accessors.push_back( accessor );

    // 4 - Update the primitive
    m_primitive.attributes[name->second] = m_document.accessors.size() - 1;
}

int addSampler( gltf::Document& document, gltf::Sampler sampler ) {
    auto s = std::find_if(
        document.samplers.cbegin(), document.samplers.cend(), [&sampler]( const gltf::Sampler& s ) {
            return ( ( s.magFilter == sampler.magFilter ) && ( s.minFilter == sampler.minFilter ) &&
                     ( s.wrapS == sampler.wrapS ) && ( s.wrapT == sampler.wrapT ) );
        } );
    if ( s == document.samplers.cend() ) {
        document.samplers.push_back( sampler );
        return document.samplers.size() - 1;
    }
    return std::distance( document.samplers.cbegin(), s );
}

int addImage( gltf::Document& document, gltf::Image img ) {
    auto s = std::find_if(
        document.images.cbegin(), document.images.cend(), [&img]( const gltf::Image& i ) {
            return ( ( !img.uri.empty() ) && ( i.uri == img.uri ) );
        } );
    if ( s == document.images.cend() ) {
        document.images.push_back( img );
        return document.images.size() - 1;
    }
    return std::distance( document.images.cbegin(), s );
}

int addTexture( gltf::Document& document,
                int /* buffer // Use this to save embeded textures */,
                const Ra::Engine::Data::TextureParameters& params ) {

    gltf::Texture texture;

    gltf::Image image;
    image.uri      = g_texturePrefix + params.name.substr( params.name.find_last_of( "/\\" ) + 1 );
    texture.source = addImage( document, std::move( image ) );

    gltf::Sampler sampler;
    sampler.magFilter = gltf::Sampler::MagFilter( (unsigned int)( params.magFilter ) );
    sampler.minFilter = gltf::Sampler::MinFilter( (unsigned int)( params.minFilter ) );
    sampler.wrapS     = gltf::Sampler::WrappingMode( (unsigned int)( params.wrapS ) );
    sampler.wrapT     = gltf::Sampler::WrappingMode( (unsigned int)( params.wrapT ) );
    texture.sampler   = addSampler( document, std::move( sampler ) );
    auto s            = std::find_if(
        document.textures.cbegin(), document.textures.cend(), [&texture]( const gltf::Texture& t ) {
            return ( ( texture.sampler == t.sampler ) && ( texture.source == t.source ) );
        } );
    if ( s == document.textures.cend() ) {
        document.textures.push_back( texture );
        return document.textures.size() - 1;
    }
    return std::distance( document.textures.cbegin(), s );
}

void addMaterialTextureExtension( gltf::Material::Texture& texNode,
                                  Engine::Data::GLTFMaterial* material,
                                  const std::string& texName ) {
    auto transform = material->getTextureTransform( texName );
    if ( transform ) {

        gltf_KHRTextureTransform tt;
        tt.offset   = transform->offset;
        tt.scale    = transform->scale;
        tt.rotation = transform->rotation;
        tt.texCoord = transform->texCoord;
        if ( !tt.isDefault() ) {
            texNode.extensionsAndExtras["extensions"]["KHR_texture_transform"] = tt;
            gltf_usedExtensions.insert( { "KHR_texture_transform" } );
        }
    }
}
void addBaseMaterial( gltf::Document& document,
                      int buffer,
                      Engine::Data::GLTFMaterial* material,
                      gltf::Material& node ) {

    node.name = material->getInstanceName();
    // generates Texture, sampler and image node for each base texture : "TEX_NORMAL",
    // "TEX_OCCLUSION", "TEX_EMISSIVE"
    {
        auto t = material->getTextureParameter( { "TEX_NORMAL" } );
        if ( t ) {
            node.normalTexture.index = addTexture( document, buffer, *t );
            node.normalTexture.scale = material->getNormalTextureScale();
            addMaterialTextureExtension(
                static_cast<fx::gltf::Material::Texture&>( node.normalTexture ),
                material,
                { "TEX_NORMAL" } );
        }
    }
    {
        auto t = material->getTextureParameter( { "TEX_OCCLUSION" } );
        if ( t ) {
            node.occlusionTexture.index    = addTexture( document, buffer, *t );
            node.occlusionTexture.strength = material->getOcclusionStrength();
            addMaterialTextureExtension(
                static_cast<fx::gltf::Material::Texture&>( node.occlusionTexture ),
                material,
                { "TEX_OCCLUSION" } );
        }
    }
    {
        auto t = material->getTextureParameter( { "TEX_EMISSIVE" } );
        if ( t ) {
            node.emissiveTexture.index = addTexture( document, buffer, *t );
            addMaterialTextureExtension(
                static_cast<fx::gltf::Material::Texture&>( node.emissiveTexture ),
                material,
                { "TEX_EMISSIVE" } );
        }
    }
    // Get the base parameters
    // Color
    const auto& c       = material->getEmissiveFactor();
    node.emissiveFactor = { c( 0 ), c( 1 ), c( 2 ) };
    // int paramaters
    node.alphaMode   = gltf::Material::AlphaMode( material->getAlphaMode() );
    node.doubleSided = material->isDoubleSided();
    // floats parameters
    node.alphaCutoff = material->getAlphaCutoff();
}

int addMetallicRoughnessMaterial( gltf::Document& document,
                                  int buffer,
                                  Engine::Data::MetallicRoughness* material ) {
    document.materials.push_back( gltf::Material {} );
    gltf::Material& node = document.materials.back();
    int idxMaterial      = document.materials.size() - 1;
    addBaseMaterial( document, buffer, material, node );

    // Base color
    {
        auto t = material->getTextureParameter( { "TEX_BASECOLOR" } );
        if ( t ) {
            node.pbrMetallicRoughness.baseColorTexture.index = addTexture( document, buffer, *t );
            addMaterialTextureExtension( static_cast<fx::gltf::Material::Texture&>(
                                             node.pbrMetallicRoughness.baseColorTexture ),
                                         material,
                                         { "TEX_BASECOLOR" } );
        }
        const auto& c                             = material->getBaseColorFactor();
        node.pbrMetallicRoughness.baseColorFactor = { c( 0 ), c( 1 ), c( 2 ), c( 3 ) };
    }
    // MetallicRoughness color
    {
        auto t = material->getTextureParameter( { "TEX_METALLICROUGHNESS" } );
        if ( t ) {
            node.pbrMetallicRoughness.metallicRoughnessTexture.index =
                addTexture( document, buffer, *t );
            addMaterialTextureExtension( static_cast<fx::gltf::Material::Texture&>(
                                             node.pbrMetallicRoughness.metallicRoughnessTexture ),
                                         material,
                                         { "TEX_METALLICROUGHNESS" } );
        }
        node.pbrMetallicRoughness.metallicFactor  = material->getMetallicFactor();
        node.pbrMetallicRoughness.roughnessFactor = material->getRoughnessFactor();
    }
    return idxMaterial;
}

int addSpecularGlossinessMaterial( gltf::Document& document,
                                   int buffer,
                                   Engine::Data::SpecularGlossiness* material ) {
    gltf_usedExtensions.insert( { "KHR_materials_pbrSpecularGlossiness" } );
    gltf_PBRSpecularGlossiness outMaterial;

    document.materials.push_back( gltf::Material {} );
    gltf::Material& node = document.materials.back();
    int idxMaterial      = document.materials.size() - 1;
    addBaseMaterial( document, buffer, material, node );

    // Diffuse color
    {
        auto t = material->getTextureParameter( { "TEX_DIFFUSE" } );
        if ( t ) {
            outMaterial.diffuseTexture.index = addTexture( document, buffer, *t );
            addMaterialTextureExtension(
                static_cast<fx::gltf::Material::Texture&>( outMaterial.diffuseTexture ),
                material,
                { "TEX_DIFFUSE" } );
        }
        const auto& c             = material->getDiffuseFactor();
        outMaterial.diffuseFactor = { c( 0 ), c( 1 ), c( 2 ), c( 3 ) };
    }
    // SpecularGlossiness color
    {
        auto t = material->getTextureParameter( { "TEX_SPECULARGLOSSINESS" } );
        if ( t ) {
            outMaterial.specularGlossinessTexture.index = addTexture( document, buffer, *t );
            addMaterialTextureExtension(
                static_cast<fx::gltf::Material::Texture&>( outMaterial.specularGlossinessTexture ),
                material,
                { "TEX_SPECULARGLOSSINESS" } );
        }
        outMaterial.glossinessFactor = material->getGlossinessFactor();
        const auto& c                = material->getSpecularFactor();
        outMaterial.specularFactor   = { c( 0 ), c( 1 ), c( 2 ) };
    }
    node.extensionsAndExtras["extensions"]["KHR_materials_pbrSpecularGlossiness"] = outMaterial;
    return idxMaterial;
}

#ifdef EXPORT_BLINNPHONG
/* TODO
 * convert blinn-phong to pbrSpecularGlossiness such as ...
 *
 *
 */
int transformBlinnPhongMaterial( gltf::Document& document,
                                 int buffer,
                                 Ra::Engine::Data::BlinnPhongMaterial* material ) {
    return -1;
}
#endif

int addMaterial( gltf::Document& document, int buffer, Ra::Engine::Data::Material* material ) {
    int materialIndex { -1 };
    // Supported Materials : MetallicRoughness, SpecularGlossiness,
    if ( material->getMaterialName() == "MetallicRoughness" ) {
        materialIndex = addMetallicRoughnessMaterial(
            document, buffer, dynamic_cast<Engine::Data::MetallicRoughness*>( material ) );
    }
    else if ( material->getMaterialName() == "SpecularGlossiness" ) {
        materialIndex = addSpecularGlossinessMaterial(
            document, buffer, dynamic_cast<Engine::Data::SpecularGlossiness*>( material ) );
    }
#ifdef EXPORT_BLINNPHONG
    else if ( material->getMaterialName() == "BlinnPhong" ) {
        materialIndex = transformBlinnPhongMaterial(
            document, buffer, dynamic_cast<Ra::Engine::Data::BlinnPhongMaterial*>( material ) );
    }
#endif
    else {
        LOG( logWARNING ) << "GLTF export : unsupported material " << material->getMaterialName();
    }
    return materialIndex;
}

int addMesh( gltf::Document& document, int buffer, Ra::Engine::Rendering::RenderObject* ro ) {
    auto displayMesh = dynamic_cast<Ra::Engine::Data::Mesh*>( ro->getMesh().get() );
    if ( !displayMesh ) { return -1; }
    const auto& geometry = displayMesh->getCoreGeometry();
    // geometry is a TriangleMesh (IndexedGeometry<Vector3ui>)  that is an
    // AttribArrayGeometry<Vector3ui>
    document.meshes.push_back( gltf::Mesh {} );
    gltf::Mesh& mesh = document.meshes.back();
    mesh.name        = displayMesh->getName();
    mesh.primitives.push_back( gltf::Primitive {} );
    gltf::Primitive& primitive = mesh.primitives.back();
    primitive.mode             = gltf::Primitive::Mode::Triangles;
    // 1 - Store the indices accessor index
    primitive.indices = addIndices( document, buffer, geometry );
    // 2 - manage vertex attributes
    const auto& vertexAttribs = geometry.vertexAttribs();
    VertexAttribWriter addAttrib { document, buffer, primitive };
    vertexAttribs.for_each_attrib( addAttrib );
    // 3 - manage material
    auto material = ro->getMaterial().get();
    if ( material ) { primitive.material = addMaterial( document, buffer, material ); }
    return document.meshes.size() - 1;
}

void addNode( gltf::Document& document, int buffer, Ra::Engine::Rendering::RenderObject* ro ) {
    // Create the node
    document.nodes.push_back( gltf::Node {} );
    gltf::Node& node = document.nodes.back();
    // Fill the node
    node.name = "node " + std::to_string( document.nodes.size() - 1 ); // ro->getName();
    fillTransform( node, ro->getLocalTransform() );
    node.mesh = addMesh( document, buffer, ro );
}

void glTFFileWriter::write( std::vector<Ra::Engine::Scene::Entity*> toExport ) {
    if ( toExport.empty() ) {
        LOG( logWARNING ) << "No entities selected : abort file save.";
        return;
    }
    if ( m_writeImages ) {
        LOG( logWARNING ) << "Texture image writing is not yet supported. exporting only texture "
                             "uri (file name) ";
    }
    g_texturePrefix = m_texturePrefix;
    auto roManager  = RadiumEngine::getInstance()->getRenderObjectManager();
    gltf::Document radiumScene;
    radiumScene.asset.generator = "Radium glTF Plugin";
    // Create the buffer and add it to the json
    int currentBuffer = 0;
    {
        gltf::Buffer sceneBuffer;
        sceneBuffer.uri        = m_bufferName;
        sceneBuffer.name       = m_rootName;
        sceneBuffer.byteLength = 0;
        radiumScene.buffers.push_back( sceneBuffer );
    }
    // Export scene
    for ( const auto e : toExport ) {
        // An entity define a scene with its root node
        radiumScene.scenes.push_back( gltf::Scene {} );
        gltf::Scene& sceneRoot = radiumScene.scenes.back();
        sceneRoot.name         = e->getName();
        sceneRoot.nodes.push_back( radiumScene.nodes.size() );

        // create the root node associated to the entity
        radiumScene.nodes.push_back( gltf::Node {} );
        int parentNode                     = radiumScene.nodes.size() - 1;
        radiumScene.nodes[parentNode].name = e->getName() + " root node.";
        // initialize the node : transformation
        fillTransform( radiumScene.nodes[parentNode], e->getTransform() );
        for ( const auto& c : e->getComponents() ) {
            // Nothing to do with the component, just loop over its renderObjects
            for ( const auto& roIdx : c->m_renderObjects ) {
                const auto& ro = roManager->getRenderObject( roIdx );
                // verify the type of the RO : do not save debug nor ui ro
                if ( ro->getType() == Ra::Engine::Rendering::RenderObjectType::Geometry &&
                     ro->isVisible() ) {
                    // Update the state of the materials
                    auto material = ro->getMaterial();
                    material->updateFromParameters();
                    // Add a new node and link to parent
                    radiumScene.nodes[parentNode].children.push_back( radiumScene.nodes.size() );
                    addNode( radiumScene, currentBuffer, ro.get() );
                }
                else {
                    LOG( logINFO ) << "\t\tRenderObject " << ro->getName()
                                   << " is not a geometry RO. Not saved";
                }
            }
        }
    }
    // Finalize the document : set the scene attribute
    radiumScene.scene = 0;
    // Add extensions used and required :
    for ( const auto& e : gltf_usedExtensions ) {
        radiumScene.extensionsUsed.push_back( e );
    }
    for ( const auto& e : gltf_requiredExtensions ) {
        radiumScene.extensionsRequired.push_back( e );
    }
    try {
        gltf::Save( radiumScene, m_fileName, false );
    }
    catch ( gltf::invalid_gltf_document& e ) {
        LOG( logERROR ) << "Caught invalid_gltf_document exception : " << e.what();
    }
    catch ( std::exception& e ) {
        LOG( logERROR ) << "Caught std::exception exception : " << e.what();
    }
}

} // namespace GLTF
} // namespace IO
} // namespace Ra
