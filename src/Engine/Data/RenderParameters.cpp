#include "Core/Containers/VariableSet.hpp"
#include <Core/Utils/Log.hpp>

#include <Engine/Data/RenderParameters.hpp>
#include <Engine/RadiumEngine.hpp>

#include <fstream>
namespace Ra {
namespace Engine {
namespace Data {

RenderParameters::StaticParameterBinder RenderParameters::s_binder;

void RenderParameters::bind( const Data::ShaderProgram* shader ) const {
    visit( s_binder, shader );
}

void ParameterSetEditingInterface::loadMetaData( const std::string& basename,
                                                 nlohmann::json& destination ) {
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };
    std::string metadataFileName = "Metadata/" + basename + ".json";
    std::ifstream metadata( resourcesRootDir + metadataFileName );
    if ( metadata ) { metadata >> destination; }
    else {
        LOG( Core::Utils::logERROR )
            << "RenderParameters : failed to load metadata file " << metadataFileName;
    }
}
} // namespace Data
} // namespace Engine
} // namespace Ra
