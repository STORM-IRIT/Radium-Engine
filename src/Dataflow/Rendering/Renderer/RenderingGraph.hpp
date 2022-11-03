#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {
using namespace Ra::Dataflow::Rendering::Nodes;
using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API RenderingGraph : public DataflowGraph
{
  public:
    explicit RenderingGraph( const std::string& name );
    ~RenderingGraph() override = default;

    // Remove the 3 following methods if there is no need to specialize
    void init() override;
    bool addNode( Node* newNode ) override;
    bool removeNode( Node* node ) override;

    // These methods are specialized to identify rendering nodes (and those who needs
    // rendertechnique)
    void clearNodes() override;
    bool compile() override;

    /// Sets the shader program manager
    void setShaderProgramManager( Ra::Engine::Data::ShaderProgramManager* shaderMngr ) {
        m_shaderMngr = shaderMngr;
    }

    /// Resize all the rendering output
    void resize( uint32_t width, uint32_t height );

    /// Set render techniques needed by the rendering nodes
    void buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const;
    /// Return the typename of the Graph
    static const std::string& getTypename();

  protected:
    bool fromJsonInternal( const nlohmann::json& data ) override;
    void toJsonInternal( nlohmann::json& ) const override;

  private:
    /// The renderer's shader program manager
    Ra::Engine::Data::ShaderProgramManager* m_shaderMngr { nullptr };
    /// List of nodes that requires some particular processing
    std::vector<RenderingNode*> m_renderingNodes; // to resize
    std::vector<RenderingNode*> m_rtIndexedNodes; // associate an index and buildRenderTechnique
};

inline RenderingGraph::RenderingGraph( const std::string& name ) :
    DataflowGraph( name, getTypename() ) {}

inline const std::string& RenderingGraph::getTypename() {
    static std::string demangledTypeName { "Rendering Graph" };
    return demangledTypeName;
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
