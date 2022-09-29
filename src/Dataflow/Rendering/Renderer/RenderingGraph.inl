#pragma once

#include <Dataflow/Rendering/Renderer/RenderingGraph.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {

inline RenderingGraph::RenderingGraph( const std::string& name ) :
    DataflowGraph( name, getTypename() ) {}

inline const std::string RenderingGraph::getTypename() {
    return "RenderingGraph";
}

inline void RenderingGraph::resize( uint32_t width, uint32_t height ) {
    for ( auto rn : m_renderingNodes ) {
        rn->resize( width, height );
    }
}

inline void RenderingGraph::setDataSources( std::vector<RenderObjectType>* ros,
                                            std::vector<LightType>* lights ) {
#if 0
    for(auto sn : m_dataProviders) {
        sn->setScene(ros, lights);
    }
#endif
}

inline void RenderingGraph::setCameras( std::vector<CameraType>* cameras ) {
#if 0
    for(auto sn : m_dataProviders) {
        sn->setCameras(cameras);
    }
#endif
}

inline void RenderingGraph::fromJsonInternal( const nlohmann::json& ) {}
inline void RenderingGraph::toJsonInternal( nlohmann::json& ) const {}

} // namespace Renderer
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
