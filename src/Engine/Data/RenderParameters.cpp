#include <Core/Utils/Log.hpp>

#include <Engine/Data/RenderParameters.hpp>
#include <Engine/RadiumEngine.hpp>

#include <fstream>
namespace Ra {
namespace Engine {
namespace Data {

RenderParameters::StaticParameterBinder RenderParameters::s_binder;

void RenderParameters::bind( const Data::ShaderProgram* shader ) const {
    m_parameterSets.visit( s_binder, shader );
}

void RenderParameters::addParameter( const std::string& name, const std::string& value ) {
    auto converterFunc = m_parameterSets.existsVariable<
        std::function<void( Core::VariableSet&, const std::string&, const std::string& )>>( name );
    if ( converterFunc ) { ( *converterFunc )->second( m_parameterSets, name, value ); }
    else {
        LOG( Core::Utils::logWARNING )
            << "RenderParameters, try to set enum value from string without converter. Adding "
               "non-bindable TParameter<string> "
            << name << " " << value;
        m_parameterSets.insertOrAssignVariable( name, value );
    }
}

void RenderParameters::addParameter( const std::string& name, const char* value ) {
    addParameter( name, std::string( value ) );
}

void RenderParameters::mergeKeepParameters( const RenderParameters& params ) {
    m_parameterSets.mergeKeepVariables( params.getStorage() );
}

void RenderParameters::mergeReplaceParameters( const RenderParameters& params ) {
    m_parameterSets.mergeReplaceVariables( params.getStorage() );
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
