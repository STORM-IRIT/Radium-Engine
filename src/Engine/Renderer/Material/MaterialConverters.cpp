#include <Engine/Renderer/Material/MaterialConverters.hpp>

#include <Core/Log/Log.hpp>

#include <map>

///////////////////////////////////////////////
////        Material converter system       ///
///////////////////////////////////////////////

namespace Ra {
namespace Engine {
///////////////////////////////////////////////
////        Radium Material converters      ///
///////////////////////////////////////////////
/*
Material* MaterialConverter::operator()( const Ra::Asset::MaterialData* toconvert ) {
    LOG( logERROR ) << "Trying to convert a abstract material ... !";
    return nullptr;
}
*/
namespace EngineMaterialConverters {
/// Map that stores each conversion function
static std::map<std::string, std::function<RadiumMaterialPtr( AssetMaterialPtr )>>
    MaterialConverterRegistry;

bool registerMaterialConverter( const std::string& name, ConverterFunction converter ) {
    auto result = MaterialConverterRegistry.insert( {name, converter} );
    return result.second;
}

bool removeMaterialConverter( const std::string& name ) {
    std::size_t removed = MaterialConverterRegistry.erase( name );
    return ( removed == 1 );
}

std::pair<bool, ConverterFunction> getMaterialConverter( const std::string& name ) {
    auto search = MaterialConverterRegistry.find( name );
    if ( search != MaterialConverterRegistry.end() )
    {
        return {true, search->second};
    }
    auto result = std::make_pair( false, [name]( AssetMaterialPtr ) -> RadiumMaterialPtr {
        LOG( logERROR ) << "Required material converter " << name << " not found!";
        return nullptr;
    } );
    return result;
}

} // namespace EngineMaterialConverters
} // namespace Engine
} // namespace Ra
