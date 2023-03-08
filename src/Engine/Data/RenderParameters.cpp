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

void RenderParameters::addEnumConverter( const std::string& name,
                                         std::shared_ptr<AbstractEnumConverter> converter ) {
    m_enumConverters[name] = converter;
}

Core::Utils::optional<std::shared_ptr<RenderParameters::AbstractEnumConverter>>
RenderParameters::getEnumConverter( const std::string& name ) {
    auto it = m_enumConverters.find( name );
    if ( it != m_enumConverters.end() ) { return it->second; }
    else {
        return {};
    }
}

std::string RenderParameters::getEnumString( const std::string& name, int value ) {
    auto it = m_enumConverters.find( name );
    if ( it != m_enumConverters.end() ) { return it->second->getEnumerator( value ); }
    else {
        return {};
    }
}

void RenderParameters::addParameter( const std::string& name, const std::string& value ) {
    auto it = m_enumConverters.find( name );
    if ( it != m_enumConverters.end() ) { it->second->setEnumValue( *this, name, value ); }
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
