// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>

// include the Engine/entity/component interface
#include <Core/Asset/BlinnPhongMaterialData.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Core/Tasks/Task.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/FrameInfo.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>

/// Compute a texture wrt to a given timestamp
class DynamicTextureSystem : public Ra::Engine::Scene::System
{
    unsigned char data[192 * 512];
    Ra::Engine::Data::TextureManager* m_texManager { nullptr };
    /// Name of the texture, used as identifier by the TextureManager
    const std::string textureName { "myTexture" };
    /// Copy of the texture parameters, used by the TextureManager to identify the pending texture
    Ra::Engine::Data::TextureParameters textureParam;

    inline void computeTexture( Scalar time ) {
        // fill with some function
        for ( int i = 0; i < 192; ++i ) {
            for ( int j = 0; j < 512; j++ ) {
                data[( i * 512 + j )] =
                    (unsigned char)( 255.0 * std::abs( std::sin( j * i * M_PI / 64.0 + time ) *
                                                       std::cos( j * i * M_PI / 96.0 + time ) ) );
            }
        }
    }

  public:
    inline DynamicTextureSystem( Ra::Engine::Data::TextureManager* mng ) : m_texManager( mng ) {
        assert( m_texManager != nullptr );

        /// Create texture
        this->computeTexture( 0 );
        auto& myTexture = m_texManager->addTexture( textureName, 192, 512, data );
        // these values will be used when engine initialize texture GL representation.
        myTexture.format         = gl::GLenum::GL_RED;
        myTexture.internalFormat = gl::GLenum::GL_R8;
    }

    virtual void generateTasks( Ra::Core::TaskQueue* q,
                                const Ra::Engine::FrameInfo& info ) override {

        // Random motion + color update wrt to position
        q->registerTask( new Ra::Core::FunctionTask(
            [info, this]() {
                this->computeTexture( info.m_animationTime );
                m_texManager->updateTextureContent( textureName, data );
            },
            "computeTexture" ) );
    }
};

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    //![Parameterize the Engine  time loop]
    app.m_engine->setEndTime( 10_ra ); // <-- 3 relates to the keyframes of the demo component.
    app.m_engine->setForwardBackward( true );
    app.m_engine->setRealTime( true );
    app.m_engine->play( true );

    //! [Create the demo system]
    DynamicTextureSystem* sys = new DynamicTextureSystem( app.m_engine->getTextureManager() );

    app.m_engine->registerSystem( "Minimal system", sys );

    //! [Creating a quad geometry with texture coordinates]
    auto quad = Ra::Core::Geometry::makeZNormalQuad( { 1_ra, 1_ra }, {}, true );
    //! [Creating a quad geometry with texture coordinates]

    //! [Create an entity and component to draw or data]
    auto e = app.m_engine->getEntityManager()->createEntity( "Textured quad" );

    Ra::Core::Asset::BlinnPhongMaterialData matData( "myMaterialData" );
    matData.m_hasTexDiffuse = true;
    // this name has to be the same as texManager added texture name
    matData.m_texDiffuse = "myTexture";

    // the entity get's this new component ownership. a bit wired since hidden in ctor.
    new Ra::Engine::Scene::TriangleMeshComponent( "Quad Mesh", e, std::move( quad ), &matData );

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    return app.exec();
}
