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

void SimpleMaterial::updateRenderingParameters() {
    auto& renderParameters = getParameters();
    // update the rendering paramaters
    renderParameters.addParameter( "material.color", m_color );
    renderParameters.addParameter( "material.perVertexColor", m_perVertexColor );
    Texture* tex = getTexture( SimpleMaterial::TextureSemantic::TEX_COLOR );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.color", tex ); }
    renderParameters.addParameter( "material.tex.hasColor", tex != nullptr );
    tex = getTexture( SimpleMaterial::TextureSemantic::TEX_MASK );
    if ( tex != nullptr ) { renderParameters.addParameter( "material.tex.mask", tex ); }
    renderParameters.addParameter( "material.tex.hasMask", tex != nullptr );
}

void SimpleMaterial::updateGL() {
    if ( !isDirty() ) { return; }

    updateRenderingParameters();
    setClean();
}

void SimpleMaterial::loadMetaData( nlohmann::json& destination ) {
    ParameterSetEditingInterface::loadMetaData( "Simple", destination );
}
} // namespace Data
} // namespace Engine
} // namespace Ra
