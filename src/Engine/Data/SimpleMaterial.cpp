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
    auto& renderParameters = getParameters();
    // update the rendering paramaters
    renderParameters.setVariable( "material.color", m_color );
    renderParameters.setVariable( "material.perVertexColor", m_perVertexColor );
    Texture* tex = getTexture( SimpleMaterial::TextureSemantic::TEX_COLOR );
    if ( tex != nullptr ) { renderParameters.setTexture( "material.tex.color", tex ); }
    renderParameters.setVariable( "material.tex.hasColor", tex != nullptr );
    tex = getTexture( SimpleMaterial::TextureSemantic::TEX_MASK );
    if ( tex != nullptr ) { renderParameters.setTexture( "material.tex.mask", tex ); }
    renderParameters.setVariable( "material.tex.hasMask", tex != nullptr );
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
