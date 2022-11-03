#include <Dataflow/Rendering/RenderingGraph.hpp>

#include <Engine/Rendering/RenderObject.hpp>
#include <Engine/Rendering/RenderTechnique.hpp>

using namespace Ra::Engine::Rendering;

namespace Ra {
namespace Dataflow {
namespace Rendering {
using namespace Ra::Dataflow::Rendering::Nodes;
using namespace Ra::Dataflow::Core;

void RenderingGraph::init() {
    DataflowGraph::init();
}

bool RenderingGraph::addNode( Node* newNode ) {
    auto added = DataflowGraph::addNode( newNode );
    if ( added ) {
        // Todo : is there something to do ?
    }
    return added;
}

bool RenderingGraph::removeNode( Node* node ) {
    auto removed = DataflowGraph::removeNode( node );
    if ( removed ) {
        // Todo : is there something to do ?
    }
    return removed;
}

bool RenderingGraph::compile() {
    m_renderingNodes.clear();
    m_rtIndexedNodes.clear();
    auto compiled = DataflowGraph::compile();
    if ( compiled ) {
        const auto& compiledNodes = getNodesByLevel();
        int idx = 1; // The renderTechnique id = 0 is reserved for ui/debug objects
        for ( const auto& lvl : *compiledNodes ) {
            for ( auto n : lvl ) {
                auto renderNode = dynamic_cast<RenderingNode*>( n );
                if ( renderNode != nullptr ) {
                    m_renderingNodes.push_back( renderNode );
                    if ( renderNode->hasRenderTechnique() ) {
                        renderNode->setIndex( idx++ );
                        m_rtIndexedNodes.push_back( renderNode );
                    }
                    renderNode->setShaderProgramManager( m_shaderMngr );
                }
            }
        }
    }
    return compiled;
}

void RenderingGraph::clearNodes() {
    DataflowGraph::clearNodes();
    m_renderingNodes.clear();
    m_rtIndexedNodes.clear();
}

void RenderingGraph::buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const {
    auto rt = std::make_shared<RenderTechnique>();
    for ( const auto& rn : m_rtIndexedNodes ) {
        rn->buildRenderTechnique( ro, *rt );
    }
    rt->updateGL();
    ro->setRenderTechnique( rt );
}

} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
