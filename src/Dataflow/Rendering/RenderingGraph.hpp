#pragma once
#include <Dataflow/RaDataflow.hpp>

#include <Dataflow/Core/DataflowGraph.hpp>

#include <Dataflow/Rendering/Nodes/RenderingNode.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {

using namespace Ra::Dataflow::Rendering::Nodes;
using namespace Ra::Dataflow::Core;

class RA_DATAFLOW_API RenderingGraph : public DataflowGraph
{
  public:
    explicit RenderingGraph( const std::string& name );
    ~RenderingGraph() override = default;

    // Remove the 3 following methods if there is no need to specialize
    void init() override;
    std::pair<bool, Node*> addNode( std::unique_ptr<Node> newNode ) override;
    bool removeNode( Node*& node ) override;

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

// -----------------------------------------------------------------
// ---------------------- inline methods ---------------------------

inline RenderingGraph::RenderingGraph( const std::string& name ) :
    DataflowGraph( name, getTypename() ) {
    // A rendering graph always use the builtin RenderingNodes factory
    addFactory( NodeFactoriesManager::getFactory( "RenderingNodes" ) );
}

inline const std::string& RenderingGraph::getTypename() {
    static std::string demangledTypeName { "Rendering Graph" };
    return demangledTypeName;
}

inline void RenderingGraph::resize( uint32_t width, uint32_t height ) {
    for ( auto rn : m_renderingNodes ) {
        rn->resize( width, height );
    }
}

inline bool RenderingGraph::fromJsonInternal( const nlohmann::json& data ) {
    auto r = DataflowGraph::fromJsonInternal( data );
    // todo, extract RenderingGraph specific data
    return r;
}

inline void RenderingGraph::toJsonInternal( nlohmann::json& data ) const {
    DataflowGraph::toJsonInternal( data );
    // todo, add RenderingGraph specific data
}

} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
