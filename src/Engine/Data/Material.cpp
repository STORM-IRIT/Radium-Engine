#include <Engine/Data/Material.hpp>

#include <Engine/RadiumEngine.hpp>

#include <fstream>

namespace Ra {
namespace Engine {
namespace Data {
Material::Material( const std::string& instanceName,
                    const std::string& materialName,
                    MaterialAspect aspect ) :
    ShaderParameterProvider(),
    m_instanceName { instanceName },
    m_aspect { aspect },
    m_materialName { materialName } {}

bool Material::isTransparent() const {
    return m_aspect == MaterialAspect::MAT_TRANSPARENT;
}

std::list<std::string> Material::getPropertyList() const {
    return ShaderParameterProvider::getPropertyList();
}

void Material::loadMetaData( const std::string& basename, nlohmann::json& destination ) {
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() };
    std::string metadataFileName = "Metadata/" + basename + ".json";
    std::ifstream metadata( resourcesRootDir + metadataFileName );
    if ( metadata ) { metadata >> destination; }
    else {
        LOG( Core::Utils::logERROR )
            << "Material : failed to load metadata file " << metadataFileName;
    }
}

} // namespace Data
} // namespace Engine
} // namespace Ra
