

#include <Dataflow/Rendering/Nodes/RenderNodes/WireframeRenderingNode.hpp>

#include <Engine/Data/Material.hpp>
#include <Engine/Data/Mesh.hpp>
#include <Engine/Data/ShaderConfigFactory.hpp>
#include <Engine/Data/ShaderProgramManager.hpp>

namespace Ra {
namespace Dataflow {
namespace Rendering {
namespace Nodes {

using namespace Ra::Engine::Data;

// Warning : this will be removed when index layers will be available on Radium
namespace WireframeNodeInternal {

template <typename IndexContainerType>
void computeIndices( Ra::Core::Geometry::LineMesh::IndexContainerType& indices,
                     IndexContainerType& other ) {

    for ( const auto& index : other ) {
        auto s = index.size();
        for ( unsigned int i = 0; i < s; ++i ) {
            int i1 = index[i];
            int i2 = index[( i + 1 ) % s];
            if ( i1 > i2 ) std::swap( i1, i2 );
            indices.emplace_back( i1, i2 );
        }
    }

    std::sort( indices.begin(),
               indices.end(),
               []( const Ra::Core::Geometry::LineMesh::IndexType& a,
                   const Ra::Core::Geometry::LineMesh::IndexType& b ) {
                   return a[0] < b[0] || ( a[0] == b[0] && a[1] < b[1] );
               } );
    indices.erase( std::unique( indices.begin(), indices.end() ), indices.end() );
}

// store LineMesh and Core, define the observer functor to update data one core update for wireframe
// linemesh
template <typename CoreGeometry>
class VerticesUpdater
{
  public:
    VerticesUpdater( std::shared_ptr<Ra::Engine::Data::LineMesh> disp, CoreGeometry& core ) :
        m_disp { disp }, m_core { core } {};

    void operator()() { m_disp->getCoreGeometry().setVertices( m_core.vertices() ); }
    std::shared_ptr<Ra::Engine::Data::LineMesh> m_disp;
    CoreGeometry& m_core;
};

template <typename CoreGeometry>
class IndicesUpdater
{
  public:
    IndicesUpdater( std::shared_ptr<Ra::Engine::Data::LineMesh> disp, CoreGeometry& core ) :
        m_disp { disp }, m_core { core } {};

    void operator()() {
        auto lineIndices = m_disp->getCoreGeometry().getIndicesWithLock();
        computeIndices( lineIndices, m_core.getIndices() );
        m_disp->getCoreGeometry().indicesUnlock();
    }
    std::shared_ptr<Ra::Engine::Data::LineMesh> m_disp;
    CoreGeometry& m_core;
};

// create a linemesh to draw wireframe given a core mesh
template <typename CoreGeometry>
void setupLineMesh( std::shared_ptr<Ra::Engine::Data::LineMesh>& disp, CoreGeometry& core ) {
    Ra::Core::Geometry::LineMesh lines;
    Ra::Core::Geometry::LineMesh::IndexContainerType indices;
    lines.setVertices( core.vertices() );
    computeIndices( indices, core.getIndices() );
    if ( indices.size() > 0 ) {
        lines.setIndices( std::move( indices ) );
        disp = std::make_shared<Ra::Engine::Data::LineMesh>( std::string( "wireframe" ),
                                                             std::move( lines ) );
        disp->updateGL();
        // add observer
        auto handle = core.template getAttribHandle<typename CoreGeometry::Point>(
            Ra::Core::Geometry::getAttribName( Ra::Core::Geometry::VERTEX_POSITION ) );

        core.vertexAttribs().getAttrib( handle ).attach( VerticesUpdater( disp, core ) );
        core.attach( IndicesUpdater( disp, core ) );
    }
    else { disp.reset(); }
}

template <typename CoreGeometry>
void processLineMesh( const std::shared_ptr<CoreGeometry>& m,
                      std::shared_ptr<Ra::Engine::Data::LineMesh>& r ) {
    if ( m->getRenderMode() ==
         Ra::Engine::Data::AttribArrayDisplayable::MeshRenderMode::RM_TRIANGLES ) {
        setupLineMesh( r, m->getCoreGeometry() );
    }
}

} // namespace WireframeNodeInternal

WireframeRenderingNode::WireframeRenderingNode( const std::string& name ) :
    RenderingNode( name, getTypename() ) {

    addInput( m_inObjects );
    m_inObjects->mustBeLinked();
    addInput( m_inCamera );
    m_inCamera->mustBeLinked();

    // TODO, allow to not provide the two following inputs, then using internal textures ?
    addInput( m_inColor );
    m_inColor->mustBeLinked();
    addInput( m_inDepth );
    m_inDepth->mustBeLinked();

    addInput( m_inActivated );

    addOutput( m_outColor, m_colorTexture );
    addEditableParameter( m_editableActivate );
}

void WireframeRenderingNode::init() {
    m_framebuffer = new globjects::Framebuffer();

    m_nodeState = new globjects::State( globjects::State::DeferredMode );
    m_nodeState->enable( gl::GL_DEPTH_TEST );
    m_nodeState->depthFunc( gl::GL_LESS );
    m_nodeState->depthMask( gl::GL_FALSE );
    m_nodeState->colorMask( gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE, gl::GL_TRUE );
    m_nodeState->blendFuncSeparate(
        gl::GL_SRC_ALPHA, gl::GL_ONE_MINUS_SRC_ALPHA, gl::GL_ONE, gl::GL_ZERO );
    // no glBlendEquationSeparate in globjects::State
    m_nodeState->enable( gl::GL_BLEND );
    m_nodeState->polygonOffset( -1.1f, -1.0f );
    m_nodeState->enable( gl::GL_POLYGON_OFFSET_FILL );
}

void WireframeRenderingNode::destroy() {
    delete m_framebuffer;

    delete m_nodeState;
}

void WireframeRenderingNode::resize( uint32_t width, uint32_t height ) {
    m_wireframeWidth  = width;
    m_wireframeHeight = height;
}

void WireframeRenderingNode::buildRenderTechnique(
    const Ra::Engine::Rendering::RenderObject* ro,
    Ra::Engine::Rendering::RenderTechnique& rt ) const {
    // TODO, use the Radium shader instead of the specific one --> wait for PR "small fixes" (#1035)
    // before that
    std::string resourcesRootDir = m_resourceDir + "Shaders/WireframeNode/";
    auto mat = const_cast<Ra::Engine::Rendering::RenderObject*>( ro )->getMaterial();
    // Volumes are not used in WireframePass
    if ( mat->getMaterialAspect() == Ra::Engine::Data::Material::MaterialAspect::MAT_DENSITY ) {
        return;
    }
    if ( auto cfg = Ra::Engine::Data::ShaderConfigurationFactory::getConfiguration(
             { "WireframeNode::WireframeRendering" } ) ) {
        rt.setConfiguration( *cfg, m_idx );
    }
    else {
        Ra::Engine::Data::ShaderConfiguration theConfig { { "WireframeNode::WireframeRendering" },
                                                          resourcesRootDir + "Wireframe.vert.glsl",
                                                          resourcesRootDir +
                                                              "Wireframe.frag.glsl" };
        theConfig.addShader( Ra::Engine::Data::ShaderType_GEOMETRY,
                             resourcesRootDir + "Wireframe.geom.glsl" );
        // Add to the ShaderConfigManager
        Ra::Engine::Data::ShaderConfigurationFactory::addConfiguration( theConfig );
        // Add to the RenderTechnique
        rt.setConfiguration( theConfig, m_idx );
    }
    rt.setParametersProvider( mat, m_idx );
}

bool WireframeRenderingNode::execute() {
    // Get parameters
    // Render objects
    auto& renderObjects = m_inObjects->getData();
    // Cameras
    auto& camera = m_inCamera->getData();

    // Color tex
    m_colorTexture = &m_inColor->getData();
    m_outColor->setData( m_colorTexture );

    // Depth buffer
    auto& depthBuffer = m_inDepth->getData();

    // activation :
    auto activated = m_inActivated->isLinked() ? m_inActivated->getData() : m_activate;

    // Compute the result
    if ( activated ) {
        if ( m_wireframes.size() > renderObjects.size() ) { m_wireframes.clear(); }

        m_framebuffer->bind();
        m_framebuffer->attachTexture( gl::GL_DEPTH_ATTACHMENT, depthBuffer.texture() );
        m_framebuffer->attachTexture( gl::GL_COLOR_ATTACHMENT0, m_colorTexture->texture() );
        const gl::GLenum buffers[] = { gl::GL_COLOR_ATTACHMENT0 };
        gl::glDrawBuffers( 1, buffers );

        auto currentState = globjects::State::currentState();
        m_nodeState->apply();
        gl::glBlendEquationSeparate( gl::GL_FUNC_ADD, gl::GL_FUNC_ADD );

        for ( const auto& ro : renderObjects ) {
            if ( ro->isVisible() ) {
                std::shared_ptr<Ra::Engine::Data::Displayable> wro;

                auto it = m_wireframes.find( ro.get() );
                if ( it == m_wireframes.end() ) {
                    using trimesh =
                        Ra::Engine::Data::IndexedGeometry<Ra::Core::Geometry::TriangleMesh>;
                    using polymesh =
                        Ra::Engine::Data::IndexedGeometry<Ra::Core::Geometry::PolyMesh>;
                    // TODO : add here quadmesh as in #925
                    std::shared_ptr<Ra::Engine::Data::LineMesh> disp;
                    auto displayable = ro->getMesh();
                    auto tm          = std::dynamic_pointer_cast<trimesh>( displayable );
                    auto tp          = std::dynamic_pointer_cast<polymesh>( displayable );
                    if ( tm ) { WireframeNodeInternal::processLineMesh( tm, disp ); }
                    if ( tp ) { WireframeNodeInternal::processLineMesh( tp, disp ); }
                    m_wireframes[ro.get()] = disp;
                    wro                    = disp;
                }
                else { wro = it->second; }

                auto shader = m_shaderMngr->getShaderProgram( "WireframeNode::WireframeRendering" );
                if ( wro && shader ) {
                    wro->updateGL();
                    shader->bind();
                    Ra::Core::Matrix4 modelMatrix = ro->getTransformAsMatrix();
                    shader->setUniform( "transform.proj", camera.projMatrix );
                    shader->setUniform( "transform.view", camera.viewMatrix );
                    shader->setUniform( "transform.model", modelMatrix );
                    shader->setUniform( "viewport",
                                        Ra::Core::Vector2 { m_wireframeWidth, m_wireframeHeight } );
                    wro->render( shader );
                }
            }
        }

        currentState->apply();
        m_framebuffer->unbind();
    }
    return true;
}
} // namespace Nodes
} // namespace Rendering
} // namespace Dataflow
} // namespace Ra
