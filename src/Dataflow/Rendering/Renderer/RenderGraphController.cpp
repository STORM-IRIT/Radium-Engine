#include <Dataflow/Rendering/Renderer/RenderGraphController.hpp>

#include <Engine/Data/ShaderProgramManager.hpp>
#include <Engine/Data/Texture.hpp>
#include <Engine/Data/ViewingParameters.hpp>

#include <Engine/Rendering/RenderObject.hpp>

#include <Engine/Scene/DefaultLightManager.hpp>

#include <globjects/Framebuffer.h>

using namespace Ra::Engine;
using namespace Ra::Engine::Scene;
using namespace Ra::Engine::Data;
using namespace Ra::Engine::Rendering;
using namespace gl;

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Renderer {
using namespace Ra::Dataflow::Core;

RenderGraphController::RenderGraphController() : ControllableRenderer::RendererController() {}

void RenderGraphController::configure( ControllableRenderer* renderer, int w, int h ) {
    ControllableRenderer::RendererController::configure( renderer, w, h );
    if ( !m_graphToLoad.empty() ) {
        loadGraph( m_graphToLoad );
        m_graphToLoad = "";
    }
}

void RenderGraphController::resize( int w, int h ) {
    ControllableRenderer::RendererController::resize( w, h );
    if ( m_renderGraph ) { m_renderGraph->resize( m_width, m_height ); }
}

void RenderGraphController::compile( bool notifyObservers ) const {
    if ( !m_renderGraph->m_ready ) {
        // compile the model
        m_renderGraph->compile();
        // notify the view the model changes
        if ( notifyObservers ) { notify(); }
        // notify the model the view may have changed
        m_renderGraph->resize( m_width, m_height );
        // fetch the data setters and getters from the graph
        m_renderGraphInputs  = m_renderGraph->getAllDataSetters();
        m_renderGraphOutputs = m_renderGraph->getAllDataGetters();
    }
}
void RenderGraphController::update( const Ra::Engine::Data::ViewingParameters& ) {
    if ( m_renderGraph ) {
        // Compile and notify the observers in case of state change.
        compile( true );
    }
}

bool RenderGraphController::buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const {
    if ( m_renderGraph ) {
        // Only the first call of compile will effectively compile the graph
        // do not notify observers here
        compile();
        m_renderGraph->buildRenderTechnique( ro );
        return true;
    }
    return false;
}

const std::vector<TextureType*>&
RenderGraphController::render( std::vector<RenderObjectPtrType>* ros,
                               std::vector<LightPtrType>* lights,
                               const CameraType& cameras ) const {
    m_images.clear();
    m_images.shrink_to_fit();

    // remove the const using const_cast as scene node expect non const object
    auto& localCamera = const_cast<CameraType&>( cameras );

    bool status = false;

    if ( m_renderGraph && m_renderGraph->m_ready ) {
        // set input data
        for ( const auto& [ptr, name, type] : m_renderGraphInputs ) {
            if ( type == simplifiedDemangledType( *ros ) ) {
                ptr->setData( ros );
                m_renderGraph->activateDataSetter( name );
            }
            if ( type == simplifiedDemangledType( *lights ) ) {
                ptr->setData( lights );
                m_renderGraph->activateDataSetter( name );
            }
            if ( type == simplifiedDemangledType( localCamera ) ) {
                ptr->setData( &localCamera );
                m_renderGraph->activateDataSetter( name );
            }
        }

        // execute the graph
        status = m_renderGraph->execute();

        if ( status ) {
            // reset the status so that it will indicate that images are available
            status = false;
            // get output
            // expect it is sufficient
            m_images.reserve( m_renderGraphOutputs.size() * 9 );
            for ( const auto& [ptr, name, type] : m_renderGraphOutputs ) {
                if ( ptr->hasData() ) {
                    status = true;
                    if ( ptr->getTypeName() == simplifiedDemangledType<TextureType*>() ) {
                        // Try a simple texture
                        auto tex = ptr->getData<TextureType*>();
                        if ( tex != nullptr ) { m_images.push_back( tex ); }
                        continue;
                    }
                    if ( ptr->getTypeName() ==
                         simplifiedDemangledType<std::vector<TextureType*>>() ) {
                        // Try a texture vector
                        const auto& texv = ptr->getData<std::vector<TextureType*>>();
                        for ( auto t : texv ) {
                            if ( t != nullptr ) { m_images.push_back( t ); }
                        }
                        continue;
                    }
                    LOG( Ra::Core::Utils::logWARNING )
                        << "Fetching from " << ptr->getName() << " (" << ptr->getTypeName()
                        << ") : type not supported !";
                }
            }
        }
    }

    if ( !status ) {
        LOG( Ra::Core::Utils::logWARNING ) << " Graph execution failed : no images generated!";
    }

    return m_images;
}

void RenderGraphController::loadGraph( const std::string& filename ) {
    auto loadedGraph =
        dynamic_cast<RenderingGraph*>( DataflowGraph::loadGraphFromJsonFile( filename ) );
    if ( loadedGraph ) {
        m_renderGraph.reset( loadedGraph );
        m_renderGraph->setShaderProgramManager( m_shaderMngr );
        notify();
    }
    else {
        LOG( Ra::Core::Utils::logERROR )
            << "RenderGraphController::loadGraph : unable to load " << filename;
    }
}

void RenderGraphController::deferredLoadGraph( const std::string& filename ) {
    m_graphToLoad = filename;
}

void RenderGraphController::saveGraph( const std::string& filename ) {
    if ( m_renderGraph ) {
        auto graphName = filename.substr( filename.find_last_of( '/' ) + 1 );
        m_renderGraph->saveToJson( filename );
        m_renderGraph->setInstanceName( graphName );
    }
}

void RenderGraphController::resetGraph() {
    m_renderGraph.release();
    m_renderGraph = std::make_unique<RenderingGraph>( "untitled" );
    m_renderGraph->setShaderProgramManager( m_shaderMngr );
}

} // namespace Renderer
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
