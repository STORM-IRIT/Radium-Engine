#include <Dataflow/Rendering/Renderer/ControllableRenderer.hpp>

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

ControllableRenderer::RendererController::RendererController() :
    Ra::Core::Resources::ObservableVoid() {}

void ControllableRenderer::RendererController::configure( ControllableRenderer* renderer,
                                                          int w,
                                                          int h ) {
    m_attachedRenderer = renderer;
    m_shaderMngr       = m_attachedRenderer->m_shaderProgramManager;
    m_width            = w;
    m_height           = h;
}

void ControllableRenderer::RendererController::resize( int w, int h ) {
    m_width  = w;
    m_height = h;
}

// ------------------------------------------------------------------------------------------
//  Interface with Radium renderer ...
// ------------------------------------------------------------------------------------------

ControllableRenderer::ControllableRenderer( RendererController& controller ) :
    Renderer(), m_controller { controller }, m_name { m_controller.getRendererName() } {
    m_controller.attachMember( this, &ControllableRenderer::controllerStateChanged );
}

ControllableRenderer::~ControllableRenderer() = default;

void ControllableRenderer::controllerStateChanged() {
    m_controllerStateChanged = true;
}

bool ControllableRenderer::buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const {
    return m_controller.buildRenderTechnique( ro );
}

void ControllableRenderer::initResources() {
    // uses several resources from the Radium engine
    auto resourcesRootDir { RadiumEngine::getInstance()->getResourcesDir() + "Shaders/" };

    m_shaderProgramManager->addShaderProgram(
        { { "Hdr2Ldr" },
          resourcesRootDir + "2DShaders/Basic2D.vert.glsl",
          resourcesRootDir + "2DShaders/Hdr2Ldr.frag.glsl" } );

    m_postprocessFbo = std::make_unique<globjects::Framebuffer>();
}

void ControllableRenderer::initializeInternal() {
    auto lmngr = dynamic_cast<DefaultLightManager*>(
        Ra::Engine::RadiumEngine::getInstance()->getSystem( "DefaultLightManager" ) );
    if ( lmngr == nullptr ) {
        lmngr = new DefaultLightManager();
        Ra::Engine::RadiumEngine::getInstance()->registerSystem( "DefaultLightManager", lmngr );
    }
    m_lightmanagers.push_back( lmngr );

    // Initialize renderer resources
    initResources();
    m_controller.configure( this, m_width, m_height );

    // TODO update shared textures as the controller modify its output : observe the controller ?
    for ( const auto& t : m_sharedTextures ) {
        m_secondaryTextures.insert( { t.first, t.second.get() } );
    }
}

void ControllableRenderer::resizeInternal() {
    // Resize the controller
    m_controller.resize( m_width, m_height );

    // Resize the internal resources
    m_postprocessFbo->bind();
    m_postprocessFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_fancyTexture->texture() );
    // finished with fbo, unbind to bind default
    globjects::Framebuffer::unbind();
}

void ControllableRenderer::updateStepInternal(
    const Ra::Engine::Data::ViewingParameters& renderData ) {
    m_controller.update( renderData );
    if ( m_controllerStateChanged ) {
        buildAllRenderTechniques();
        m_controllerStateChanged = false;
    }

    // TODO, improve light and camera management to prevent multiple alloc/copy ...
    auto lightMngr = getLightManager();
    auto lights    = getLights();
    lights->clear();
    lights->reserve( lightMngr->count() );
    for ( size_t i = 0; i < lightMngr->count(); i++ ) {
        lights->push_back( lightMngr->getLight( i ) );
    }
}

void ControllableRenderer::renderInternal( const Ra::Engine::Data::ViewingParameters& renderData ) {

    const auto& renderings = m_controller.render( allRenderObjects(), getLights(), &renderData );
    if ( renderings.size() > 0 ) {
        m_colorTexture = renderings[0]; // allow to select which image to fetch
    }
    else {
        m_colorTexture = nullptr;
    }
}

void ControllableRenderer::postProcessInternal( const Ra::Engine::Data::ViewingParameters& ) {
    if ( m_colorTexture ) {
        m_postprocessFbo->bind();

        // GL_ASSERT( glDrawBuffers( 1, buffers ) );
        GL_ASSERT( glDrawBuffer( GL_COLOR_ATTACHMENT0 ) );
        GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
        GL_ASSERT( glDepthMask( GL_FALSE ) );

        auto shader = m_postProcessEnabled
                          ? m_shaderProgramManager->getShaderProgram( "Hdr2Ldr" )
                          : m_shaderProgramManager->getShaderProgram( "DrawScreen" );
        shader->bind();
        shader->setUniform( "screenTexture", m_colorTexture, 0 );
        m_quadMesh->render( shader );

        GL_ASSERT( glDepthMask( GL_TRUE ) );
        GL_ASSERT( glEnable( GL_DEPTH_TEST ) );

        m_postprocessFbo->unbind();
    }
}

void ControllableRenderer::debugInternal( const Ra::Engine::Data::ViewingParameters& ) {}

void ControllableRenderer::uiInternal( const Ra::Engine::Data::ViewingParameters& ) {}

/*
 *                   *************
 */

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

void RenderGraphController::update( const Ra::Engine::Data::ViewingParameters& ) {
    if ( m_renderGraph && !m_renderGraph->m_ready ) {
        // compile the model
        m_renderGraph->compile();
        // notify the view the model changes
        notify();
        // notify the model the view may have changed
        m_renderGraph->resize( m_width, m_height );

        // fetch the data setters and getters from the graph
        m_renderGraphInputs  = m_renderGraph->getAllDataSetters();
        m_renderGraphOutputs = m_renderGraph->getAllDataGetters();
    }
}

bool RenderGraphController::buildRenderTechnique( Ra::Engine::Rendering::RenderObject* ro ) const {
    if ( m_renderGraph ) {
        if ( !m_renderGraph->m_ready ) {
            m_renderGraph->compile();
            m_renderGraph->resize( m_width, m_height );
            // fetch the data setters and getters from the graph
            m_renderGraphInputs  = m_renderGraph->getAllDataSetters();
            m_renderGraphOutputs = m_renderGraph->getAllDataGetters();
        }
        if ( m_renderGraph->m_ready ) {
            m_renderGraph->buildRenderTechnique( ro );
            return true;
        }
    }
    return false;
}

const std::vector<TextureType*>&
RenderGraphController::render( std::vector<RenderObjectPtrType>* ros,
                               std::vector<LightPtrType>* lights,
                               const CameraType* cameras ) const {
    m_images.clear();
    m_images.shrink_to_fit();
    if ( m_renderGraph && m_renderGraph->m_ready ) {
        // set input data
        for ( const auto& [ptr, name, type] : m_renderGraphInputs ) {
            if ( type == simplifiedDemangledType( *ros ) ) { ptr->setData( ros ); }
            if ( type == simplifiedDemangledType( *lights ) ) { ptr->setData( lights ); }
            if ( type == simplifiedDemangledType( *cameras ) ) { ptr->setData( cameras ); }
        }

        // execute the graph
        m_renderGraph->execute();

        // get output
        // expect it is sufficient
        m_images.reserve( m_renderGraphOutputs.size() * 4 );
        for ( const auto& [ptr, name, type] : m_renderGraphOutputs ) {
            auto tex = ptr->getData<TextureType*>();
            if ( tex != nullptr ) { m_images.push_back( tex ); }
        }
        LOG( Ra::Core::Utils::logINFO ) << " Graph executed. Got " << m_images.size() << " images!";
    }
    else {
        LOG( Ra::Core::Utils::logWARNING ) << " Graph not compiled : no images generated!";
    }
    return m_images;
}

void RenderGraphController::loadGraph( const std::string& filename ) {
    m_renderGraph.release();
    auto graphName = filename.substr( filename.find_last_of( '/' ) + 1 );
    m_renderGraph  = std::make_unique<RenderingGraph>( graphName );
    m_renderGraph->setShaderProgramManager( m_shaderMngr );
    m_renderGraph->loadFromJson( filename );
    notify();
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
