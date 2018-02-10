#include <Engine/Renderer/Renderer.hpp>

#include <globjects/Framebuffer.h>

#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/Math/ColorPresets.hpp>
#include <Core/Mesh/MeshUtils.hpp>
#include <Core/Mesh/MeshPrimitives.hpp>

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Light/Light.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Renderer/Light/PointLight.hpp>
#include <Engine/Renderer/Light/SpotLight.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/Texture/TextureManager.hpp>
#include <Engine/Renderer/Texture/Texture.hpp>
#include <Engine/Renderer/RenderObject/RenderObjectManager.hpp>
#include <Engine/Renderer/RenderObject/RenderObject.hpp>

namespace Ra {
    namespace Engine {
        namespace {
            const GLenum buffers[] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3,
                GL_COLOR_ATTACHMENT4,
                GL_COLOR_ATTACHMENT5,
                GL_COLOR_ATTACHMENT6,
                GL_COLOR_ATTACHMENT7
            };
        }

        Renderer::Renderer( )
            : m_width( 0 )
            , m_height( 0 )
            , m_shaderMgr( nullptr )
            , m_displayedTexture( nullptr )
            , m_renderQueuesUpToDate( false )
            , m_quadMesh( nullptr )
            , m_drawDebug( true )
            , m_wireframe( false )
            , m_postProcessEnabled( true )
            , m_brushRadius( 0 )
        {
            GL_CHECK_ERROR;
        }

        Renderer::~Renderer()
        {
            ShaderProgramManager::destroyInstance();
        }

        void Renderer::initialize(uint width, uint height)
        {
            m_width  = width;
            m_height = height;

            // Initialize managers
            m_shaderMgr = ShaderProgramManager::getInstance();
            m_roMgr = RadiumEngine::getInstance()->getRenderObjectManager();
            TextureManager::createInstance();

            m_shaderMgr->addShaderProgram("DrawScreen", "Shaders/Basic2D.vert.glsl", "Shaders/DrawScreen.frag.glsl");
            m_shaderMgr->addShaderProgram("DrawScreenI", "Shaders/Basic2D.vert.glsl", "Shaders/DrawScreenI.frag.glsl");
            m_shaderMgr->addShaderProgram("CircleBrush", "Shaders/Basic2D.vert.glsl", "Shaders/CircleBrush.frag.glsl");

            ShaderConfiguration pickingPointsConfig( "PickingPoints" );
            pickingPointsConfig.addShader(ShaderType_VERTEX  , "Shaders/Picking.vert.glsl");
            pickingPointsConfig.addShader(ShaderType_GEOMETRY, "Shaders/PickingPoints.geom.glsl");
            pickingPointsConfig.addShader(ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl");
            ShaderConfigurationFactory::addConfiguration( pickingPointsConfig );
            m_pickingShaders[0] = m_shaderMgr->addShaderProgram( pickingPointsConfig );

            ShaderConfiguration pickingLinesConfig( "PickingLines" );
            pickingLinesConfig.addShader(ShaderType_VERTEX  , "Shaders/Picking.vert.glsl");
            pickingLinesConfig.addShader(ShaderType_GEOMETRY, "Shaders/PickingLines.geom.glsl");
            pickingLinesConfig.addShader(ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl");
            ShaderConfigurationFactory::addConfiguration( pickingLinesConfig );
            m_pickingShaders[1] = m_shaderMgr->addShaderProgram( pickingLinesConfig );

            ShaderConfiguration pickingTrianglesConfig( "PickingTriangles" );
            pickingTrianglesConfig.addShader(ShaderType_VERTEX  , "Shaders/Picking.vert.glsl");
            pickingTrianglesConfig.addShader(ShaderType_GEOMETRY, "Shaders/PickingTriangles.geom.glsl");
            pickingTrianglesConfig.addShader(ShaderType_FRAGMENT, "Shaders/Picking.frag.glsl");
            ShaderConfigurationFactory::addConfiguration( pickingTrianglesConfig );
            m_pickingShaders[2] = m_shaderMgr->addShaderProgram( pickingTrianglesConfig );

            m_depthTexture.reset(new Texture("Depth"));
            m_depthTexture->internalFormat = GL_DEPTH_COMPONENT24;
            m_depthTexture->dataType = GL_UNSIGNED_INT;

            m_pickingFbo.reset( new globjects::Framebuffer() );

            m_pickingTexture.reset(new Texture("Picking"));
            m_pickingTexture->internalFormat = GL_RGBA32I;
            m_pickingTexture->dataType = GL_INT;
            m_pickingTexture->minFilter = GL_NEAREST;
            m_pickingTexture->magFilter = GL_NEAREST;

            // Final texture
            m_fancyTexture.reset(new Texture( "Final"));
            m_fancyTexture->internalFormat = GL_RGBA32F;
            m_fancyTexture->dataType = GL_FLOAT;

            m_displayedTexture = m_fancyTexture.get();
            m_secondaryTextures["Picking Texture"] = m_pickingTexture.get();

            // Quad mesh
            Core::TriangleMesh mesh = Core::MeshUtils::makeZNormalQuad(Core::Vector2( -1.f, 1.f));

            m_quadMesh.reset( new Mesh( "quad" ) );
            m_quadMesh->loadGeometry( mesh );
            m_quadMesh->updateGL();

            initializeInternal();

            resize( m_width, m_height );
        }

        void Renderer::render( const RenderData& data )
        {
            CORE_ASSERT( RadiumEngine::getInstance() != nullptr, "Engine is not initialized." );

            std::lock_guard<std::mutex> renderLock( m_renderMutex );
            CORE_UNUSED( renderLock );

            m_timerData.renderStart = Core::Timer::Clock::now();

            // 0. Save eventual already bound FBO (e.g. QtOpenGLWidget) and viewport
            saveExternalFBOInternal();

            // 1. Gather render objects if needed
            feedRenderQueuesInternal( data );

            m_timerData.feedRenderQueuesEnd = Core::Timer::Clock::now();

            // 2. Update them (from an opengl point of view)
            // FIXME(Charly): Maybe we could just update objects if they need it
            // before drawing them, that would be cleaner (performance problem ?)
            updateRenderObjectsInternal( data );
            m_timerData.updateEnd = Core::Timer::Clock::now();

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
            m_timerData.mainRenderEnd = Core::Timer::Clock::now();

            // 5. Post processing
            postProcessInternal( data );
            m_timerData.postProcessEnd = Core::Timer::Clock::now();

            // 6. Debug
            debugInternal( data );

            // 7. Draw UI
            uiInternal( data );

            // 8. Write image to Qt framebuffer.
            drawScreenInternal();
            m_timerData.renderEnd = Core::Timer::Clock::now();

            // 9. Tell renderobjects they have been drawn (to decreaase the counter)
            notifyRenderObjectsRenderingInternal();
        }

        void Renderer::saveExternalFBOInternal()
        {
            // Save the current viewport ...
            glGetIntegerv(GL_VIEWPORT, m_qtViewport);
            // save the currently bound FBO
            GL_ASSERT( glGetIntegerv( GL_FRAMEBUFFER_BINDING, &m_qtPlz ) );
            // Set the internal rendering viewport
            glViewport(0, 0, m_width, m_height);
        }

        void Renderer::updateRenderObjectsInternal( const RenderData& renderData )
        {
            for (auto &ro : m_fancyRenderObjects)
            {
                ro->updateGL();
            }
            for (auto &ro : m_xrayRenderObjects)
            {
                ro->updateGL();
            }
            for (auto &ro : m_debugRenderObjects)
            {
                ro->updateGL();
            }
            for (auto &ro : m_uiRenderObjects)
            {
                ro->updateGL();
            }
        }

        void Renderer::feedRenderQueuesInternal( const RenderData& renderData )
        {
            m_fancyRenderObjects.clear();
            m_debugRenderObjects.clear();
            m_uiRenderObjects.clear();
            m_xrayRenderObjects.clear();

            m_roMgr->getRenderObjectsByType( renderData, m_fancyRenderObjects, RenderObjectType::Fancy );
            m_roMgr->getRenderObjectsByType( renderData, m_debugRenderObjects, RenderObjectType::Debug );
            m_roMgr->getRenderObjectsByType( renderData, m_uiRenderObjects,    RenderObjectType::UI );

            for ( auto it = m_fancyRenderObjects.begin(); it != m_fancyRenderObjects.end(); )
            {
                if ( (*it)->isXRay() )
                {
                    m_xrayRenderObjects.push_back( *it );
                    it = m_fancyRenderObjects.erase( it );
                }
                else
                {
                    ++it;
                }
            }

            for ( auto it = m_debugRenderObjects.begin(); it != m_debugRenderObjects.end(); )
            {
                if ( (*it)->isXRay() )
                {
                    m_xrayRenderObjects.push_back( *it );
                    it = m_debugRenderObjects.erase( it );
                }
                else
                {
                    ++it;
                }
            }

            for ( auto it = m_uiRenderObjects.begin(); it != m_uiRenderObjects.end(); )
            {
                if ( (*it)->isXRay() )
                {
                    m_xrayRenderObjects.push_back( *it );
                    it = m_uiRenderObjects.erase( it );
                }
                else
                {
                    ++it;
                }
            }
        }

        // subroutine to Renderer::splitRenderQueuesForPicking()
        void Renderer::splitRQ( const std::vector<RenderObjectPtr>& renderQueue,
                                std::array<std::vector<RenderObjectPtr>,3>& renderQueuePicking )
        {
            // clean renderQueuePicking
            for (uint i=0; i<renderQueuePicking.size(); ++i)
            {
                renderQueuePicking[i].clear();
            }
            // fill renderQueuePicking from renderQueue
            for (auto it = renderQueue.begin(); it != renderQueue.end(); ++it)
            {
                switch ((*it)->getMesh()->getRenderMode())
                {
                case Mesh::RM_POINTS:
                {
                    renderQueuePicking[0].push_back( *it );
                    break;
                }
///\todo Fix picking for line meshes
//               case Mesh::RM_LINES:
//                case Mesh::RM_LINES_ADJACENCY:
//                case Mesh::RM_LINE_STRIP_ADJACENCY: // fall through
//                {
//                    renderQueuePicking[1].push_back( *it );
//                    break;
//                }
                case Mesh::RM_TRIANGLES:
                {
                    renderQueuePicking[2].push_back( *it );
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
        }

        void Renderer::splitRenderQueuesForPicking( const RenderData& renderData )
        {
            splitRQ( m_fancyRenderObjects, m_fancyRenderObjectsPicking );
            splitRQ( m_debugRenderObjects, m_debugRenderObjectsPicking );
            splitRQ( m_uiRenderObjects   , m_uiRenderObjectsPicking );
            splitRQ( m_xrayRenderObjects , m_xrayRenderObjectsPicking );
        }

        // subroutine to Renderer::doPicking()
        void Renderer::renderForPicking( const RenderData& renderData,
                                         const std::array<const ShaderProgram*,3>& pickingShaders,
                                         const std::array<std::vector<RenderObjectPtr>,3>& renderQueuePicking )
        {
            for (uint i = 0; i < pickingShaders.size(); ++i)
            {
                pickingShaders[i]->bind();
                pickingShaders[i]->setUniform("eltType", i); // FIXME (Florian): this does not apply!
                if (i==1)
                {
                    pickingShaders[i]->setUniform("lineWidth", 10);
                }

                for ( const auto& ro : renderQueuePicking[i] )
                {
                    if ( ro->isVisible() && ro->isPickable() )
                    {
                        int id = ro->idx.getValue();
                        pickingShaders[i]->setUniform( "objectId", id );

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        pickingShaders[i]->setUniform( "transform.proj", renderData.projMatrix );
                        pickingShaders[i]->setUniform( "transform.view", renderData.viewMatrix );
                        pickingShaders[i]->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->getMaterial()->bind( pickingShaders[i] );

                        // render
                        ro->getMesh()->render();
                    }
                }
            }
        }

        void Renderer::doPicking( const RenderData& renderData )
        {
            m_pickingResults.reserve( m_pickingQueries.size() );

            m_pickingFbo->bind();

            GL_ASSERT( glDepthMask( GL_TRUE ) );
            GL_ASSERT( glColorMask( 1, 1, 1, 1 ) );
            GL_ASSERT( glDrawBuffers( 1, buffers ) );

            float clearDepth = 1.0;
            int clearColor[] = { -1, -1, -1, -1 };

            GL_ASSERT(glClearBufferiv(GL_COLOR, 0, clearColor));
            GL_ASSERT(glClearBufferfv(GL_DEPTH, 0, &clearDepth));

            splitRenderQueuesForPicking( renderData );

            // First draw Fancy Objects
            GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
            GL_ASSERT( glDepthFunc( GL_LESS ) );

            renderForPicking( renderData, m_pickingShaders, m_fancyRenderObjectsPicking );

            // Then draw debug objects
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            if ( m_drawDebug )
            {
                renderForPicking( renderData, m_pickingShaders, m_debugRenderObjectsPicking );
            }

            // Then draw xrayed objects on top of normal objects
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            if ( m_drawDebug )
            {
                renderForPicking( renderData, m_pickingShaders, m_xrayRenderObjectsPicking );
            }

            // Finally draw ui stuff on top of everything
            // these have a different way to compute the transform matrices
            // FIXME (florian): find a way to use renderForPicking()!
            GL_ASSERT( glClear( GL_DEPTH_BUFFER_BIT ) );
            for (uint i = 0; i < m_pickingShaders.size(); ++i)
            {
                m_pickingShaders[i]->bind();
                m_pickingShaders[i]->setUniform("eltType", i);
                if (i==0)
                {
                    m_pickingShaders[i]->setUniform("pointCloudSplatRadius", 10);
                }
                if (i==1)
                {
                    m_pickingShaders[i]->setUniform("lineWidth", 10);
                }

                for ( const auto& ro : m_uiRenderObjectsPicking[i] )
                {
                    if ( ro->isVisible() && ro->isPickable() )
                    {
                        int id = ro->idx.getValue();
                        m_pickingShaders[i]->setUniform( "objectId", id );

                        Core::Matrix4 M = ro->getTransformAsMatrix();
                        Core::Matrix4 MV = renderData.viewMatrix * M;
                        Scalar d = MV.block<3, 1>( 0, 3 ).norm();

                        Core::Matrix4 S = Core::Matrix4::Identity();
                        S( 0, 0 ) = S( 1, 1 ) = S( 2, 2 ) = d;

                        M = M * S;

                        m_pickingShaders[i]->setUniform( "transform.proj", renderData.projMatrix );
                        m_pickingShaders[i]->setUniform( "transform.view", renderData.viewMatrix );
                        m_pickingShaders[i]->setUniform( "transform.model", M );

                        ro->getRenderTechnique()->getMaterial()->bind( m_pickingShaders[i] );

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
                if (query.m_mode < C_VERTEX)
                {
                    // skip query if out of window (can occur when picking while moving outside)
                    if ( query.m_screenCoords.x() < 0 || query.m_screenCoords.x() > m_width-1 ||
                         query.m_screenCoords.y() < 0 || query.m_screenCoords.y() > m_height-1 )
                    {
                        result.m_roIdx = -1;
                        m_pickingResults.push_back( result );
                        continue;
                    }
                    GL_ASSERT( glReadPixels( query.m_screenCoords.x(), query.m_screenCoords.y(),
                                             1, 1, GL_RGBA_INTEGER, GL_INT, pick ) );
                    result.m_roIdx = pick[0];                   // RO idx
                    result.m_vertexIdx.emplace_back( pick[1] ); // vertex idx in the element
                    result.m_elementIdx.emplace_back( pick[2] ); // element idx
                    result.m_edgeIdx.emplace_back( pick[3] ); // edge opposite idx for triangles
                }
                else
                {
                    // select the results for the RO with the most representatives
                    // (or first to come if same amount)
                    std::map<int,PickingResult> resultPerRO;
                    for(int i=-m_brushRadius; i<=m_brushRadius; i+=3)
                    {
                        int h = std::round( std::sqrt( m_brushRadius*m_brushRadius - i*i ) );
                        for(int j=-h; j<=+h; j+=3)
                        {
                            const int x = query.m_screenCoords.x()+i;
                            const int y = query.m_screenCoords.y()-j;
                            // skip query if out of window (can occur when picking while moving outside)
                            if ( x < 0 || x > m_width-1 || y < 0 || y > m_height-1 )
                            {
                                continue;
                            }
                            GL_ASSERT( glReadPixels( x, y, 1, 1, GL_RGBA_INTEGER, GL_INT, pick ) );
                            resultPerRO[ pick[0] ].m_roIdx = pick[0];
                            resultPerRO[ pick[0] ].m_vertexIdx.emplace_back( pick[1] );
                            resultPerRO[ pick[0] ].m_elementIdx.emplace_back( pick[2] );
                            resultPerRO[ pick[0] ].m_edgeIdx.emplace_back( pick[3] );
                        }
                    }
                    int maxRO = -1;
                    int nbMax = 0;
                    for (const auto& res : resultPerRO)
                    {
                        if (res.second.m_roIdx == -1)
                        {
                            continue;
                        }
                        if (res.second.m_vertexIdx.size() > nbMax)
                        {
                            maxRO = res.first;
                            nbMax = res.second.m_vertexIdx.size();
                        }
                    }
                    result = resultPerRO[ maxRO ];
                }
                result.m_mode = query.m_mode;
                m_pickingResults.push_back( result );
            }

            m_pickingFbo->unbind();
        }

        void Renderer::drawScreenInternal()
        {
            glViewport(m_qtViewport[0], m_qtViewport[1], m_qtViewport[2], m_qtViewport[3]);

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

            GL_ASSERT( glDepthFunc( GL_ALWAYS ) );

            auto shader = (m_displayedTexture->dataType == GL_INT ||
                           m_displayedTexture->dataType == GL_UNSIGNED_INT) ?
                                m_shaderMgr->getShaderProgram("DrawScreenI") :
                                m_shaderMgr->getShaderProgram("DrawScreen");
            shader->bind();
            shader->setUniform( "screenTexture", m_displayedTexture, 0 );
            m_quadMesh->render();

            GL_ASSERT( glDepthFunc( GL_LESS ) );

            // draw brush circle if enabled
            if( m_brushRadius>0 )
            {
                GL_ASSERT( glDisable( GL_BLEND ) );
                GL_ASSERT( glDisable( GL_DEPTH_TEST ) );
                auto shader = m_shaderMgr->getShaderProgram("CircleBrush");
                shader->bind();
                shader->setUniform("mousePosition", m_mousePosition);
                shader->setUniform("brushRadius", m_brushRadius);
                shader->setUniform("dim", Core::Vector2(m_width,m_height));
                m_quadMesh->render();
                GL_ASSERT( glEnable( GL_DEPTH_TEST ) );
                GL_ASSERT( glEnable( GL_BLEND ) );
            }

        }

        void Renderer::notifyRenderObjectsRenderingInternal()
        {
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

        void Renderer::resize( uint w, uint h )
        {
            m_width = w;
            m_height = h;

            m_depthTexture->Generate(m_width, m_height, GL_DEPTH_COMPONENT);
            m_pickingTexture->Generate(m_width, m_height, GL_RGBA_INTEGER);
            m_fancyTexture->Generate(m_width, m_height, GL_RGBA);

            m_pickingFbo->bind();
            m_pickingFbo->attachTexture( GL_DEPTH_ATTACHMENT , m_depthTexture.get()->texture() );
            m_pickingFbo->attachTexture( GL_COLOR_ATTACHMENT0, m_pickingTexture.get()->texture() );
            if ( m_pickingFbo->checkStatus() != GL_FRAMEBUFFER_COMPLETE )
            {
                LOG( logERROR ) << "FBO Error : " << m_pickingFbo->checkStatus();
            }
            m_pickingFbo->unbind();
            GL_CHECK_ERROR;

            resizeInternal();

            glDrawBuffer( GL_BACK ) ;
            glReadBuffer( GL_BACK ) ;

            GL_CHECK_ERROR;
        }

        void Renderer::displayTexture( const std::string& texName )
        {
            if ( m_secondaryTextures.find( texName) != m_secondaryTextures.end() )
            {
                m_displayedTexture = m_secondaryTextures[texName];
            }
            else
            {
                m_displayedTexture = m_fancyTexture.get();
            }
        }

        std::vector<std::string> Renderer::getAvailableTextures() const
        {
            std::vector<std::string> ret;
            ret.push_back( "Fancy Texture" );
            for ( const auto& tex : m_secondaryTextures )
            {
                ret.push_back( tex.first );
            }
            return ret;
        }

        void Renderer::reloadShaders()
        {
            ShaderProgramManager::getInstance()->reloadAllShaderPrograms();
        }

        void Renderer::handleFileLoading(const Asset::FileData &filedata)
        {
            if (! filedata.hasLight() )
            {
                return;
            }

            std::vector<  Asset::LightData * > data = filedata.getLightData();
            uint i = 0;
            for (auto light : data )
            {
                if (light->getLight())
                {
                    addLight( light->getLight() );
                    ++i;
                }
            }
            LOG(logINFO) << "Added " << i << " lights in the renderer";
            if (data.size() > i)
            {
                LOG(logWARNING) << data.size()-i << " lights where of unknown or unsupported type.";
            }
        }

        uchar *Renderer::grabFrame(uint &w, uint &h) const
        {
            Engine::Texture* tex = getDisplayTexture();
            tex->bind();

            // Get a buffer to store the pixels of the OpenGL texture (in float format)
            float* pixels = new float[tex->width() * tex->height() * 4];

            // Grab the texture data
            GL_ASSERT(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels));

            // Now we must convert the floats to RGB while flipping the image updisde down.
            uchar* writtenPixels = new uchar[tex->width() * tex->height() * 4];
            for (uint j = 0; j < tex->height(); ++j)
            {
                for (uint i = 0; i < tex->width(); ++i)
                {
                    uint in = 4 * (j * tex->width() + i);  // Index in the texture buffer
                    uint ou = 4 * ((tex->height() - 1 - j) * tex->width() + i); // Index in the final image (note the j flipping).

                    writtenPixels[ou + 0] = (uchar)Ra::Core::Math::clamp<Scalar>(pixels[in + 0] * 255.f, 0, 255);
                    writtenPixels[ou + 1] = (uchar)Ra::Core::Math::clamp<Scalar>(pixels[in + 1] * 255.f, 0, 255);
                    writtenPixels[ou + 2] = (uchar)Ra::Core::Math::clamp<Scalar>(pixels[in + 2] * 255.f, 0, 255);
                    writtenPixels[ou + 3] = 0xff;
                }
            }
            delete[] pixels;
            w = tex->width();
            h = tex->height();
            return writtenPixels;
        }

    }
} // namespace Ra
