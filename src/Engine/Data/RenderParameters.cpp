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

void RenderParameters::setEnumVariable( const std::string& name, const std::string& value ) {
    auto converterFunc = existsVariable<
        std::function<void( Core::VariableSet&, const std::string&, const std::string& )>>( name );
    if ( converterFunc ) {
        ( *converterFunc )->second( dynamic_cast<Core::VariableSet&>( *this ), name, value );
    }
    else {
        LOG( Core::Utils::logWARNING )
            << "RenderParameters, try to set enum value from string without converter. Adding "
               "non-bindable TParameter<string> "
            << name << " " << value;
        setVariable( name, value );
    }
}

void RenderParameters::setEnumVariable( const std::string& name, const char* value ) {
    setEnumVariable( name, std::string( value ) );
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
