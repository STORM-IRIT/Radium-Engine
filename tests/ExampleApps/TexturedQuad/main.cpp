// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/Viewer.hpp>

// include the Engine/entity/component interface
#include <Core/Asset/BlinnPhongMaterialData.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>
//#include <Core/Asset/TextureData.hpp>
#include <Core/Asset/TextureData.hpp>
#include <Core/Asset/TextureDataManager.hpp>

#include <QTimer>

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    //! [Creating a quad geometry with texture coordinates]
    auto quad = Ra::Core::Geometry::makeZNormalQuad( { 1_ra, 1_ra }, {}, true );
    //! [Creating a quad geometry with texture coordinates]

    //! [Creating a texture]
    constexpr int tWidth  = 192;
    constexpr int tHeight = 512;
    constexpr int tSize   = tWidth * tHeight;
    unsigned char data[tSize];
    // fill with some function
    for ( int i = 0; i < tWidth; ++i ) {
        for ( int j = 0; j < tHeight; j++ ) {
            data[( i * tHeight + j )] =
                (unsigned char)( 255.0 * std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                                   std::cos( j * i * M_PI / 96.0 ) ) );
        }
    }
    auto& textureParameters =
        app.m_engine->getTextureManager()->addTexture( "myTexture", tWidth, tHeight, data );
    // these values will be used when engine initialize texture GL representation.
    textureParameters.format         = gl::GLenum::GL_RED;
    textureParameters.internalFormat = gl::GLenum::GL_R8;
    //! [Creating a texture]

    //! [Create an entity and component to draw or data]
    auto e = app.m_engine->getEntityManager()->createEntity( "Textured quad" );

    int nChannel = 4;
    int baseSize = 1;
    Ra::Core::Asset::ImageSpec imgSpec(tWidth, tHeight, nChannel, Ra::Core::Asset::TypeUInt8);
    Ra::Core::Asset::TextureData textureData(imgSpec, data);
    Ra::Core::Asset::TextureDataManager::addTexture("myTexture", textureData);

    std::string imageFilename = "myFile";
    Ra::Core::Asset::TextureData textureData2(imageFilename);
    Ra::Core::Asset::TextureDataManager::addTexture("myTexture2", textureData2);

    Ra::Core::Asset::BlinnPhongMaterialData matData( "myMaterialData" );
    // remove glossy highlight
    matData.m_specular    = Ra::Core::Utils::Color::Black();
    matData.m_hasSpecular = true;

    matData.m_hasTexDiffuse = true;
    // this name has to be the same as texManager added texture name
    matData.m_texDiffuse = "myTexture";

    // the entity get's this new component ownership. a bit wired since hidden in ctor.
    new Ra::Engine::Scene::TriangleMeshComponent( "Quad Mesh", e, std::move( quad ), &matData );
    //! [Create an entity and component to draw or data]

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    // terminate the app after 4 second (approximatively). Camera can be moved using mouse moves.
    auto close_timer = new QTimer( &app );
    close_timer->setInterval( 100 );
    int dec = 0;

    //    auto thread = std::thread([&app, &dec]() { // not worked
    QObject::connect( close_timer, &QTimer::timeout, [&app, &dec, &textureData]() {
        unsigned char newData[tSize];
        for ( int i = 0; i < tWidth; ++i ) {
            for ( int j = 0; j < tHeight; j++ ) {
                newData[( i * tHeight + j )] =
                    (unsigned char)( 200.0 * std::abs( std::sin( float( dec ) / 4_ra ) ) *
                                     std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                               std::cos( j * i * M_PI / 96.0 ) ) );
            }
        }

        Ra::Engine::Data::TextureParameters textureParameters;
        textureParameters.name = "myTexture";
        auto texture3 = app.m_engine->getTextureManager()->getOrLoadTexture( textureParameters );

        //        texture3->updateData( newData ); // not worked

        auto& params = texture3->getParameters();
        memcpy( params.texels, newData, tSize );
        app.m_mainWindow->getViewer()->makeCurrent();
        texture3->initializeGL( false );
        app.m_mainWindow->getViewer()->doneCurrent();

        textureData.update(newData);

        std::cout << "update data with dec = " << dec << std::endl;
        ++dec;
        if ( dec == 40 ) app.appNeedsToQuit();
    } );
    close_timer->start();

    return app.exec();
}
