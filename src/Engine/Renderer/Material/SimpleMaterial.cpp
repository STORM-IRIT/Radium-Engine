#include <Engine/Renderer/Material/SimpleMaterial.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

namespace Ra {
namespace Engine {

SimpleMaterial::SimpleMaterial( const std::string& instanceName,
                                const std::string& materialName,
                                MaterialAspect aspect ) :
    Material( instanceName, materialName, aspect ) {}

SimpleMaterial::~SimpleMaterial() {
    m_textures.clear();
}

void SimpleMaterial::updateRenderingParameters() {
    // update the rendering paramaters
    m_renderParameters.addParameter( "material.color", m_color );
    m_renderParameters.addParameter( "material.perVertexColor", m_perVertexColor );
    Texture* tex = getTexture( SimpleMaterial::TextureSemantic::TEX_COLOR );
    if ( tex != nullptr )
    {
        m_renderParameters.addParameter( "material.tex.color", tex );
        m_renderParameters.addParameter( "material.tex.hasColor", 1 );
    }
    else
    { m_renderParameters.addParameter( "material.tex.hasColor", 0 ); }
    tex = getTexture( SimpleMaterial::TextureSemantic::TEX_MASK );
    if ( tex != nullptr )
    {
        m_renderParameters.addParameter( "material.tex.mask", tex );
        m_renderParameters.addParameter( "material.tex.hasMask", 1 );
    }
    else
    { m_renderParameters.addParameter( "material.tex.hasMask", 0 ); }
}

void SimpleMaterial::updateGL() {
    if ( !m_isDirty ) { return; }
    // Load textures
    TextureManager* texManager = TextureManager::getInstance();
    for ( const auto& tex : m_pendingTextures )
    {
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

} // namespace Engine
} // namespace Ra
