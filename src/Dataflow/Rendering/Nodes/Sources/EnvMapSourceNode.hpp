#pragma once
#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/Nodes/Sources/SingleDataSourceNode.hpp>

#include <Core/Utils/StdFilesystem.hpp>
#include <Engine/Data/EnvironmentTexture.hpp>

namespace Ra {
namespace Dataflow {

namespace Rendering {
using namespace Ra::Engine::Data;
using EnvmapPtrType = std::shared_ptr<EnvironmentTexture>;

namespace Nodes {

using EnvMapSourceNode = Core::Sources::SingleDataSourceNode<EnvmapPtrType>;

}
} // namespace Rendering

namespace Core {
namespace Sources {
template <>
inline const std::string& SingleDataSourceNode<Rendering::EnvmapPtrType>::getTypename() {
    static std::string typeName { "Source<Environment>" };
    return typeName;
}

template <>
inline SingleDataSourceNode<Rendering::EnvmapPtrType>::SingleDataSourceNode(
    const std::string& name ) :
    SingleDataSourceNode( name, SingleDataSourceNode<Rendering::EnvmapPtrType>::getTypename() ) {
    setEditable( "EnvMap" );
}

template <>
inline void
SingleDataSourceNode<Rendering::EnvmapPtrType>::toJsonInternal( nlohmann::json& data ) const {
    auto envTex = getData();
    if ( envTex ) {
        data["files"]    = ( *envTex )->getImageName().c_str();
        data["strength"] = ( *envTex )->getStrength() * 100.;
    }
}

template <>
inline bool
SingleDataSourceNode<Rendering::EnvmapPtrType>::fromJsonInternal( const nlohmann::json& data ) {
    if ( data.contains( "files" ) ) {
        std::string files = data["files"];
        float strength    = data["strength"];
        float s           = strength / 100.;
        // check if the file exists
        bool envmap_exist;
        auto pos = files.find( ';' );
        if ( pos != std::string::npos ) {
            std::string f1 = files.substr( 0, pos - 1 );
            envmap_exist   = std::filesystem::exists( f1 );
        }
        else {
            envmap_exist = std::filesystem::exists( files );
        }
        if ( envmap_exist ) {
            auto envmp = std::make_shared<Rendering::EnvironmentTexture>( files, true );
            envmp->setStrength( s );
            setData( envmp );
        }
        else {
            setData( nullptr );
        }
    }
    else {
        setData( nullptr );
    }
    return true;
}

} // namespace Sources
} // namespace Core
} // namespace Dataflow
} // namespace Ra
