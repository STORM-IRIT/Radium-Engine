#include <Engine/Renderer/Renderer.hpp>

#include <globjects/Framebuffer.h>

#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/File/FileData.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Camera/Camera.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>

#include <Engine/Managers/LightManager/LightManager.hpp>


namespace Ra {
namespace Engine {
namespace {
const GLenum buffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,
                          GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5,
                          GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
}

Renderer::Renderer() : m_quadMesh{nullptr},
                       m_depthTexture { nullptr },
                       m_fancyTexture {nullptr},
                       m_pickingFbo { nullptr },
                       m_pickingTexture { nullptr } {
    GL_CHECK_ERROR;
}

Renderer::~Renderer() {
    ShaderProgramManager::destroyInstance();
}

void Renderer::initialize( uint width, uint height ) {
    m_width = width;
    m_height = height;

    // Initialize managers
    m_shaderMgr = ShaderProgramManager::getInstance();
    m_roMgr = RadiumEngine::getInstance()->getRenderObjectManager();
    TextureManager::createInstance();


    m_shaderMgr->addShaderProgram( { {"DrawScreen"}, {"Shaders/Basic2D.vert.glsl"},
                                     {"Shaders/DrawScreen.frag.glsl"} }
                                 );
    m_shaderMgr->addShaderProgram( { {"DrawScreenI"}, {"Shaders/Basic2D.vert.glsl"},
                                     {"Shaders/DrawScreenI.frag.glsl"} }
                                 );
    m_shaderMgr->addShaderProgram( { {"CircleBrush"}, {"Shaders/Basic2D.vert.glsl"},
                                     {"Shaders/CircleBrush.frag.glsl"} }
                                 );

    m_shaderMgr->addShaderProgram( { {"DisplayDepthBuffer"}, {"Shaders/Basic2D.vert.glsl"},
                                     {"Shaders/DepthDisplay/DepthDisplay.frag.glsl"} }
                                 );

    ShaderConfiguration pickingPointsConfig( "PickingPoints" );
    pickingPointsConfig.addShader( ShaderType_VERTEX, "Shaders/Picking.vert.glsl" );
    pickingPointsConfig.addShader( ShaderType_GEOMETRY, "Shaders/PickingPoints.geom.glsl" );
    pickingPointsConfig.addShader( ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl" );
    ShaderConfigurationFactory::addConfiguration( pickingPointsConfig );
    m_pickingShaders[0] = m_shaderMgr->addShaderProgram( pickingPointsConfig );

    ShaderConfiguration pickingLinesConfig( "PickingLines" );
    pickingLinesConfig.addShader( ShaderType_VERTEX, "Shaders/Picking.vert.glsl" );
    pickingLinesConfig.addShader( ShaderType_GEOMETRY, "Shaders/PickingLines.geom.glsl" );
    pickingLinesConfig.addShader( ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl" );
    ShaderConfigurationFactory::addConfiguration( pickingLinesConfig );
    m_pickingShaders[1] = m_shaderMgr->addShaderProgram( pickingLinesConfig );

    ShaderConfiguration pickingLinesAdjacencyConfig( "PickingLinesAdjacency" );
    pickingLinesAdjacencyConfig.addShader( ShaderType_VERTEX, "Shaders/Picking.vert.glsl" );
    pickingLinesAdjacencyConfig.addShader( ShaderType_GEOMETRY,
                                           "Shaders/PickingLinesAdjacency.geom.glsl" );
    pickingLinesAdjacencyConfig.addShader( ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl" );
    ShaderConfigurationFactory::addConfiguration( pickingLinesAdjacencyConfig );
    m_pickingShaders[2] = m_shaderMgr->addShaderProgram( pickingLinesAdjacencyConfig );

    ShaderConfiguration pickingTrianglesConfig( "PickingTriangles" );
    pickingTrianglesConfig.addShader( ShaderType_VERTEX, "Shaders/Picking.vert.glsl" );
    pickingTrianglesConfig.addShader( ShaderType_GEOMETRY, "Shaders/PickingTriangles.geom.glsl" );
    pickingTrianglesConfig.addShader( ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl" );
    ShaderConfigurationFactory::addConfiguration( pickingTrianglesConfig );
    m_pickingShaders[3] = m_shaderMgr->addShaderProgram( pickingTrianglesConfig );

    TextureParameters texparams;
    texparams.width = m_width;
    texparams.height = m_height;
    texparams.target = GL_TEXTURE_2D;
    texparams.minFilter = GL_NEAREST;
    texparams.magFilter = GL_NEAREST;

    texparams.name = "Depth";
    texparams.internalFormat = GL_DEPTH_COMPONENT24;
    texparams.format = GL_DEPTH_COMPONENT;
    texparams.type = GL_UNSIGNED_INT;
    m_depthTexture = std::make_unique<Texture>(texparams);

    m_pickingFbo = std::make_unique<globjects::Framebuffer>();
    texparams.name = "Picking";
    texparams.internalFormat = GL_RGBA32I;
    texparams.format = GL_RGBA_INTEGER;
    texparams.type = GL_INT;
    m_pickingTexture = std::make_unique<Texture>(texparams);

    // Final texture
    texparams.name = "Final image";
    texparams.internalFormat = GL_RGBA32F;
    texparams.format = GL_RGBA;
    texparams.type = GL_FLOAT;
    m_fancyTexture = std::make_unique<Texture>(texparams);


    m_displayedTexture = m_fancyTexture.get();
    m_secondaryTextures["Picking Texture"] = m_pickingTexture.get();

    // Quad mesh
    Core::TriangleMesh mesh = Core::MeshUtils::makeZNormalQuad( Core::Vector2( -1.f, 1.f ) );

    m_quadMesh = std::make_unique<Mesh>("quad");
    m_quadMesh->loadGeometry( mesh );
    m_quadMesh->updateGL();

    initializeInternal();

    resize( m_width, m_height );

    glDrawBuffer( GL_BACK );
    glReadBuffer( GL_BACK );

}

void Renderer::render( const ViewingParameters& data ) {
    CORE_ASSERT( RadiumEngine::getInstance() != nullptr, "Engine is not initialized." );

    std::lock_guard<std::mutex> renderLock( m_renderMutex );
    CORE_UNUSED( renderLock );

    m_timerData.renderStart = Core::Utils::Clock::now();

    // 0. Save eventual already bound FBO (e.g. QtOpenGLWidget) and viewport
    saveExternalFBOInternal();

    // 1. Gather render objects if needed
    feedRenderQueuesInternal( data );

    m_timerData.feedRenderQueuesEnd = Core::Utils::Clock::now();

    // 2. Update them (from an opengl point of view)
    // FIXME(Charly): Maybe we could just update objects if they need it
    // before drawing them, that would be cleaner (performance problem ?)
    updateRenderObjectsInternal( data );
    m_timerData.updateEnd = Core::Utils::Clock::now();

    // 3. Do picking if needed
    m_pickingResults.clear();
    if ( !m_pickingQueries.empty() )
    {
        doPicking( data );
    }
    m_lastFramePickingQueries = m_pickingQueries;
    m_pickingQueries.clear();

    updateStepInternal( data );

    // 4. Do the rendering.
    renderInternal( data );
    m_timerData.mainRenderEnd = Core::Utils::Clock::now();

    // 5. Post processing
    postProcessInternal( data );
    m_timerData.postProcessEnd = Core::Utils::Clock::now();

    // 6. Debug
    debugInternal( data );

    // 7. Draw UI
    uiInternal( data );

    // 8. Write image to Qt framebuffer.
    drawScreenInternal();
    m_timerData.renderEnd = Core::Utils::Clock::now();

    // 9. Tell renderobjects they have been drawn (to decreaase the counter)
    notifyRenderObjectsRenderingInternal();
}

void Renderer::saveExternalFBOInternal() {
    // Save the current viewport ...
    glGetIntegerv( GL_VIEWPORT, m_qtViewport );
    // save the currently bound FBO
    GL_ASSERT( glGetIntegerv( GL_FRAMEBUFFER_BINDING, &m_qtPlz ) );
    // Set the internal rendering viewport
    glViewport( 0, 0, m_width, m_height );
}

void Renderer::updateRenderObjectsInternal( const ViewingParameters& renderData ) {
    for ( auto& ro : m_fancyRenderObjects )
    {
        ro->updateGL();
    }
    for ( auto& ro : m_xrayRenderObjects )
    {
        ro->updateGL();
    }
    for ( auto& ro : m_debugRenderObjects )
    {
        ro->updateGL();
    }
    for ( auto& ro : m_uiRenderObjects )
    {
        ro->updateGL();
    }
}

void Renderer::feedRenderQueuesInternal( const ViewingParameters& renderData ) {
    m_fancyRenderObjects.clear();
    m_debugRenderObjects.clear();
    m_uiRenderObjects.clear();
    m_xrayRenderObjects.clear();

    m_roMgr->getRenderObjectsByType( m_fancyRenderObjects, RenderObjectType::Geometry );
    m_roMgr->getRenderObjectsByType( m_debugRenderObjects, RenderObjectType::Debug );
    m_roMgr->getRenderObjectsByType( m_uiRenderObjects, RenderObjectType::UI );

    for ( auto it = m_fancyRenderObjects.begin(); it != m_fancyRenderObjects.end(); )
    {
        if ( ( *it )->isXRay() )
        {
            m_xrayRenderObjects.push_back( *it );
            it = m_fancyRenderObjects.erase( it );
        } else
        { ++it; }
    }

    for ( auto it = m_debugRenderObjects.begin(); it != m_debugRenderObjects.end(); )
    {
        if ( ( *it )->isXRay() )
        {
            m_xrayRenderObjects.push_back( *it );
            it = m_debugRenderObjects.erase( it );
        } else
        { ++it; }
    }

    for ( auto it = m_uiRenderObjects.begin(); it != m_uiRenderObjects.end(); )
    {
        if ( ( *it )->isXRay() )
        {
            m_xrayRenderObjects.push_back( *it );
            it = m_uiRenderObjects.erase( it );
        } else
        { ++it; }
    }
}

// subroutine to Renderer::splitRenderQueuesForPicking()
void Renderer::splitRQ( const std::vector<RenderObjectPtr>& renderQueue,
                        std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking ) {
    // clean renderQueuePicking
    for (auto &q : renderQueuePicking) {
        q.clear();
    }

    // fill renderQueuePicking from renderQueue
    for (auto &roPtr : renderQueue ) {
        switch ( roPtr->getMesh()->getRenderMode() )
        {
        case Mesh::RM_POINTS:
        {
            renderQueuePicking[0].push_back( roPtr );
            break;
        }
        case Mesh::RM_LINES:     // fall through
            [[fallthrough]];
        case Mesh::RM_LINE_LOOP: // fall through
            [[fallthrough]];
        case Mesh::RM_LINE_STRIP:
        {
            renderQueuePicking[1].push_back( roPtr );
            break;
        }
        case Mesh::RM_LINES_ADJACENCY: // fall through
        case Mesh::RM_LINE_STRIP_ADJACENCY:
        {
            renderQueuePicking[2].push_back( roPtr );
            break;
        }
        case Mesh::RM_TRIANGLES:
            [[fallthrough]];
        case Mesh::RM_TRIANGLE_STRIP:
            [[fallthrough]];
        case Mesh::RM_TRIANGLE_FAN:
        {
            renderQueuePicking[3].push_back( roPtr );
            break;
        }
        default:
        { break; }
        }
    }
}

void Renderer::splitRenderQueuesForPicking( const ViewingParameters& renderData ) {
    splitRQ( m_fancyRenderObjects, m_fancyRenderObjectsPicking );
    splitRQ( m_debugRenderObjects, m_debugRenderObjectsPicking );
    splitRQ( m_uiRenderObjects, m_uiRenderObjectsPicking );
    splitRQ( m_xrayRenderObjects, m_xrayRenderObjectsPicking );
}

// subroutine to Renderer::doPicking()
void Renderer::renderForPicking(
    const ViewingParameters& renderData, const std::array<const ShaderProgram*, 4>& pickingShaders,
    const std::array<std::vector<RenderObjectPtr>, 4>& renderQueuePicking ) {
    for ( uint i = 0; i < pickingShaders.size(); ++i )
    {
        pickingShaders[i]->bind();
        pickingShaders[i]->setUniform( "transform.proj", renderData.projMatrix );
        pickingShaders[i]->setUniform( "transform.view", renderData.viewMatrix );
        for ( const auto& ro : renderQueuePicking[i] )
        {
            if ( ro->isVisible() && ro->isPickable() )
            {
                pickingShaders[i]->setUniform( "objectId", ro->idx.getValue() );
                Core::Matrix4 M = ro->getTransformAsMatrix();
                Core::Matrix4 N = M.inverse().transpose();
                pickingShaders[i]->setUniform( "transform.model", M );
                pickingShaders[i]->setUniform( "transform.worldNormal", N );

                // render
                ro->getMesh()->render();
            }
        }
    }
}

void Renderer::doPicking( const ViewingParameters& renderData ) {
    m_pickingResults.reserve( m_pickingQueries.size() );

    m_pickingFbo->bind();

    GL_ASSERT( glDepthMask( GL_TRUE ) );
    GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
    GL_ASSERT( glDrawBuffers( 1, buffers ) );

    float clearDepth = 1.0;
    int clearColor[] = {-1, -1, -1, -1};

    GL_ASSERT( glClearBufferiv( GL_COLOR, 0, clearColor ) );
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );

    splitRenderQueuesForPicking( renderData );

    // First draw Geometry Objects
    GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
    GL_ASSERT( glDepthFunc( GL_LESS ) );

    renderForPicking( renderData, m_pickingShaders, m_fancyRenderObjectsPicking );

    // Then draw debug objects
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
    if ( m_drawDebug )
    {
        renderForPicking( renderData, m_pickingShaders, m_debugRenderObjectsPicking );
    }

    // Then draw xrayed objects on top of normal objects
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
    if ( m_drawDebug )
    {
        renderForPicking( renderData, m_pickingShaders, m_xrayRenderObjectsPicking );
    }

    // Finally draw ui stuff on top of everything
    // these have a different way to compute the transform matrices
    // FIXME (florian): find a way to use renderForPicking()!
    GL_ASSERT( glClearBufferfv( GL_DEPTH, 0, &clearDepth ) );
    for ( uint i = 0; i < m_pickingShaders.size(); ++i )
    {
        m_pickingShaders[i]->bind();
        m_pickingShaders[i]->setUniform( "transform.proj", renderData.projMatrix );
        m_pickingShaders[i]->setUniform( "transform.view", renderData.viewMatrix );

        for ( const auto& ro : m_uiRenderObjectsPicking[i] )
        {
            if ( ro->isVisible() && ro->isPickable() )
            {
                m_pickingShaders[i]->setUniform( "objectId", ro->idx.getValue() );

                Core::Matrix4 M = ro->getTransformAsMatrix();
                Core::Matrix4 MV = renderData.viewMatrix * M;
                Scalar d = MV.block<3, 1>( 0, 3 ).norm();

                Core::Matrix4 S = Core::Matrix4::Identity();
                S( 0, 0 ) = S( 1, 1 ) = S( 2, 2 ) = d;

                M = M * S;
                Core::Matrix4 N = M.inverse().transpose();

                m_pickingShaders[i]->setUniform( "transform.model", M );
                m_pickingShaders[i]->setUniform( "transform.worldNormal", N );

                // render
                ro->getMesh()->render();
            }
        }
    }

    // Now read the Picking Texture to address the Picking Requests.
    GL_ASSERT( glReadBuffer( GL_COLOR_ATTACHMENT0 ) );

    int pick[4];
    for ( const PickingQuery& query : m_pickingQueries )
    {
        PickingResult result;
        // fill picking result according to picking mode
        if ( query.m_mode < C_VERTEX )
        {
            // skip query if out of window (can occur when picking while moving outside)
            if ( query.m_screenCoords.x() < 0 || query.m_screenCoords.x() > m_width - 1 ||
                 query.m_screenCoords.y() < 0 || query.m_screenCoords.y() > m_height - 1 )
            {
                result.m_roIdx = -1;
                m_pickingResults.push_back( result );
                continue;
            }
            GL_ASSERT( glReadPixels( query.m_screenCoords.x(), query.m_screenCoords.y(), 1, 1,
                                     GL_RGBA_INTEGER, GL_INT, pick ) );
            result.m_roIdx = pick[0];                    // RO idx
            result.m_vertexIdx.emplace_back( pick[1] );  // vertex idx in the element
            result.m_elementIdx.emplace_back( pick[2] ); // element idx
            result.m_edgeIdx.emplace_back( pick[3] );    // edge opposite idx for triangles
        } else
        {
            // select the results for the RO with the most representatives
            // (or first to come if same amount)
            std::map<int, PickingResult> resultPerRO;
            for ( auto i = -m_brushRadius; i <= m_brushRadius; i += 3 )
            {
                auto h = std::round( std::sqrt( m_brushRadius * m_brushRadius - i * i ) );
                for ( auto j = -h; j <= +h; j += 3 )
                {
                    const int x = query.m_screenCoords.x() + i;
                    const int y = query.m_screenCoords.y() - j;
                    // skip query if out of window (can occur when picking while moving outside)
                    if ( x < 0 || x > m_width - 1 || y < 0 || y > m_height - 1 )
                    {
                        continue;
                    }
                    GL_ASSERT( glReadPixels( x, y, 1, 1, GL_RGBA_INTEGER, GL_INT, pick ) );
                    resultPerRO[pick[0]].m_roIdx = pick[0];
                    resultPerRO[pick[0]].m_vertexIdx.emplace_back( pick[1] );
                    resultPerRO[pick[0]].m_elementIdx.emplace_back( pick[2] );
                    resultPerRO[pick[0]].m_edgeIdx.emplace_back( pick[3] );
                }
            }
            int maxRO = -1;
            int nbMax = 0;
            for ( const auto& res : resultPerRO )
            {
                if ( res.second.m_roIdx == -1 )
                {
                    continue;
                }
                if ( res.second.m_vertexIdx.size() > nbMax )
                {
                    maxRO = res.first;
                    nbMax = res.second.m_vertexIdx.size();
                }
            }
            result = resultPerRO[maxRO];
        }
        result.m_mode = query.m_mode;
        m_pickingResults.push_back( result );
    }

    m_pickingFbo->unbind();
}

void Renderer::drawScreenInternal() {
    glViewport( m_qtViewport[0], m_qtViewport[1], m_qtViewport[2], m_qtViewport[3] );

    if ( m_qtPlz == 0 )
    {
        GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, 0 ) );
        glDrawBuffer( GL_BACK );
    }
    else
    {
        GL_ASSERT( glBindFramebuffer( GL_FRAMEBUFFER, m_qtPlz ) );
        GL_ASSERT( glDrawBuffers( 1, buffers ) );
    }
    // Display the final screen
    {
        GL_ASSERT(glDepthFunc(GL_ALWAYS));

        auto shader = (m_displayedTexture->m_textureParameters.type == GL_INT ||
                       m_displayedTexture->m_textureParameters.type == GL_UNSIGNED_INT)
                      ? ( m_displayedTexture->m_textureParameters.format == GL_DEPTH_COMPONENT
                                      ? m_shaderMgr->getShaderProgram("DisplayDepthBuffer")
                                      : m_shaderMgr->getShaderProgram("DrawScreenI") )
                      :  m_shaderMgr->getShaderProgram("DrawScreen");
        shader->bind();
        shader->setUniform("screenTexture", m_displayedTexture, 0);
        m_quadMesh->render();

        GL_ASSERT(glDepthFunc(GL_LESS));
    }
    // draw brush circle if enabled
    if ( m_brushRadius > 0 )
    {
        GL_ASSERT( glDisable( GL_BLEND ) );
        GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
        auto shader = m_shaderMgr->getShaderProgram( "CircleBrush" );
        shader->bind();
        shader->setUniform( "mousePosition", m_mousePosition );
        shader->setUniform( "brushRadius", m_brushRadius );
        shader->setUniform( "dim", Core::Vector2( m_width, m_height ) );
        m_quadMesh->render();
        GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
        GL_ASSERT( glEnable( GL_BLEND ) );
    }

}

void Renderer::notifyRenderObjectsRenderingInternal() {
    for ( auto& ro : m_fancyRenderObjects )
    {
        ro->hasBeenRenderedOnce();
    }

    for ( auto& ro : m_debugRenderObjects )
    {
        ro->hasBeenRenderedOnce();
    }

    for ( auto& ro : m_xrayRenderObjects )
    {
        ro->hasBeenRenderedOnce();
    }

    for ( auto& ro : m_uiRenderObjects )
    {
        ro->hasBeenRenderedOnce();
    }
}

void Renderer::resize( uint w, uint h ) {
    m_width = w;
    m_height = h;
    m_depthTexture->resize(m_width, m_height);
    m_pickingTexture->resize(m_width, m_height);
    m_fancyTexture->resize(m_width, m_height);

    m_pickingFbo->bind();
    m_pickingFbo->attachTexture( GL_DEPTH_ATTACHMENT, m_depthTexture->texture() );
    m_pickingFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_pickingTexture->texture() );
    if ( m_pickingFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
    {
        LOG( logERROR ) << "File " << __FILE__ << "(" << __LINE__ << ") Picking FBO Error " << m_pickingFbo->checkStatus();
    }
    m_pickingFbo->unbind();

    resizeInternal();
}

void Renderer::displayTexture( const std::string& texName ) {
    if ( m_secondaryTextures.find( texName ) != m_secondaryTextures.end() )
    {
        m_displayedTexture = m_secondaryTextures[texName];
    } else
    { m_displayedTexture = m_fancyTexture.get(); }
}

std::vector<std::string> Renderer::getAvailableTextures() const {
    std::vector<std::string> ret;
    ret.emplace_back( "Final image" );
    std::transform(m_secondaryTextures.begin(), m_secondaryTextures.end(), std::back_inserter(ret),
        [](const std::pair<std::string, Texture*> tex){return tex.first;}
        );
    return ret;
}

void Renderer::reloadShaders() {
    ShaderProgramManager::getInstance()->reloadAllShaderPrograms();
}

std::unique_ptr<uchar[]> Renderer::grabFrame(size_t &w, size_t &h) const {
    Engine::Texture* tex = getDisplayTexture();
    tex->bind();

    // Get a buffer to store the pixels of the OpenGL texture (in float format)
    auto pixels = std::unique_ptr<float[]>( new float[tex->width() * tex->height() * 4] );

    // Grab the texture data
    GL_ASSERT( glGetTexImage( GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels.get() ) );

    // Now we must convert the floats to RGB while flipping the image updisde down.
    auto writtenPixels = std::unique_ptr<uchar[]>( new uchar[tex->width() * tex->height() * 4] );
    for ( uint j = 0; j < tex->height(); ++j )
    {
        for ( uint i = 0; i < tex->width(); ++i )
        {
            auto in = 4 * ( j * tex->width() + i ); // Index in the texture buffer
            auto ou = 4 * ( ( tex->height() - 1 - j ) * tex->width() +
                            i ); // Index in the final image (note the j flipping).

            writtenPixels[ou + 0] =
                (uchar)Ra::Core::Math::clamp<Scalar>( pixels[in + 0] * 255.f, 0, 255 );
            writtenPixels[ou + 1] =
                (uchar)Ra::Core::Math::clamp<Scalar>( pixels[in + 1] * 255.f, 0, 255 );
            writtenPixels[ou + 2] =
                (uchar)Ra::Core::Math::clamp<Scalar>( pixels[in + 2] * 255.f, 0, 255 );
            writtenPixels[ou + 3] =
                (uchar)Ra::Core::Math::clamp<Scalar>( pixels[in + 3] * 255.f, 0, 255 );
        }
    }
    w = tex->width();
    h = tex->height();
    return writtenPixels;
}

void Renderer::addLight( const Light* light ) {
    for ( auto m : m_lightmanagers )
        m->addLight( light );
}

bool Renderer::hasLight() const {
    int n = 0;
    for ( auto m : m_lightmanagers )
        n += m->count();
    return n != 0;
}
} // namespace Engine
} // namespace Ra
