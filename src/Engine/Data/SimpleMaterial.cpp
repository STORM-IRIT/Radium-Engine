#include <Engine/Data/SimpleMaterial.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/RadiumEngine.hpp>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {
SimpleMaterial::SimpleMaterial( const std::string& instanceName,
                                const std::string& materialName,
                                MaterialAspect aspect ) :
    Material( instanceName, materialName, aspect ) {}

SimpleMaterial::~SimpleMaterial() {
    m_textures.clear();
}

void SimpleMaterial::updateRenderingParameters() {
    auto& renderParamaters = getParameters();
    // update the rendering paramaters
    renderParamaters.addParameter( "material.color", m_color );
    renderParamaters.addParameter( "material.perVertexColor", m_perVertexColor );
    Texture* tex = getTexture( SimpleMaterial::TextureSemantic::TEX_COLOR );
    if ( tex != nullptr ) { renderParamaters.addParameter( "material.tex.color", tex ); }
    renderParamaters.addParameter( "material.tex.hasColor", tex != nullptr );
    tex = getTexture( SimpleMaterial::TextureSemantic::TEX_MASK );
    if ( tex != nullptr ) { renderParamaters.addParameter( "material.tex.mask", tex ); }
    renderParamaters.addParameter( "material.tex.hasMask", tex != nullptr );
}

void SimpleMaterial::updateGL() {
    if ( !m_isDirty ) { return; }
    // Load textures
    Data::TextureManager* texManager = RadiumEngine::getInstance()->getTextureManager();
    for ( const auto& tex : m_pendingTextures ) {
        // ask to convert color textures from sRGB to Linear RGB
        bool tolinear         = ( tex.first == TextureSemantic::TEX_COLOR );
        auto texture          = texManager->getOrLoadTexture( tex.second, tolinear );
        m_textures[tex.first] = texture;
    }
    // as all the pending textures where initialized, clear the pending textures list
    m_pendingTextures.clear();
    m_isDirty = false;
    updateRenderingParameters();
}

void SimpleMaterial::loadMetaData( nlohmann::json& destination ) {
    ParameterSetEditingInterface::loadMetaData( "Simple", destination );
}
} // namespace Data
} // namespace Engine
} // namespace Ra
