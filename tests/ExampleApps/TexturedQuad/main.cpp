// Include Radium base application and its simple Gui
#include <Gui/BaseApplication.hpp>
#include <Gui/RadiumWindow/SimpleWindowFactory.hpp>
#include <Gui/Viewer/Viewer.hpp>

// include the Engine/entity/component interface
#include <Core/Asset/BlinnPhongMaterialData.hpp>
#include <Core/Asset/Image.hpp>
#include <Core/Asset/ImageManager.hpp>
#include <Core/Geometry/MeshPrimitives.hpp>
#include <Engine/Data/TextureManager.hpp>
#include <Engine/Rendering/RenderObjectManager.hpp>
#include <Engine/Scene/EntityManager.hpp>
#include <Engine/Scene/GeometryComponent.hpp>
#include <Engine/Scene/GeometrySystem.hpp>

#include <QTimer>
#include <future>
#include <thread>

// Vertex shader source code
const std::string _vertexShaderSource {
    "#include \"TransformStructs.glsl\"\n"
    "layout (location = 0) in vec3 in_position;\n"
    "layout (location = 4) in vec3 in_texcoord;\n"
    "layout (location = 0) out vec3 out_pos;\n"
    "layout (location = 2) out vec3 out_texcoord;\n"
    "uniform Transform transform;\n"
    "void main(void)\n"
    "{\n"
    "    mat4 mvp    = transform.proj * transform.view * transform.model;\n"
    "    out_pos     = in_position;\n"
    "    gl_Position = mvp*vec4(in_position.xyz, 1.0);\n"
    "    out_texcoord = in_texcoord;\n"
    "}\n" };

//  Fragment shader source code
const std::string _fragmentShaderSource {
    "#include \"BlinnPhong.glsl\"\n"
    "#include \"VertexAttribInterface.frag.glsl\"\n"
    "layout (location = 0) out vec4 out_color;\n"
    "uniform vec4 aColorUniform;\n"
    "uniform float aScalarUniform;\n"
    "void main(void)\n"
    "{\n"
    "   out_color =  vec4(texture(material.tex.kd, in_texcoord.xy).rgb, 1.0);\n"
    "   //out_color =  vec4(in_texcoord.xy, 1.0, 1.0);\n"
    "   // out_color =  ( 1 + cos( 20 * ( in_pos.x + aScalarUniform ) ) ) * 0.5 * aColorUniform;\n"
    "}\n" };

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    std::shared_ptr<Ra::Engine::Rendering::RenderObject> ro, ro2;

    ///////////////////////////////// create texture 1 /////////////////////////////////////
    //! [Creating a texture]
    constexpr int tWidth  = 192;
    constexpr int tHeight = 512;
    constexpr int tSize   = tWidth * tHeight;
    {
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

        Ra::Core::Asset::BlinnPhongMaterialData matData( "myMaterialData" );
        // remove glossy highlight
        matData.m_specular    = Ra::Core::Utils::Color::Black();
        matData.m_hasSpecular = true;

        matData.m_hasTexDiffuse = true;
        // this name has to be the same as texManager added texture name
        matData.m_texDiffuse = "myTexture";

        //! [Creating a quad geometry with texture coordinates]
        auto quad = Ra::Core::Geometry::makeZNormalQuad( { 1_ra, 1_ra }, {}, true );
        //! [Creating a quad geometry with texture coordinates]

        //! [Create an entity and component to draw or data]
        auto e = app.m_engine->getEntityManager()->createEntity( "Textured quad" );
        // the entity get's this new component ownership. a bit wired since hidden in ctor.
        auto c = new Ra::Engine::Scene::TriangleMeshComponent(
            "Quad Mesh", e, std::move( quad ), &matData );
        //! [Create an entity and component to draw or data]
        ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
            c->m_renderObjects[0] );
        ro->setLocalTransform( Ra::Core::Transform(
            Ra::Core::Translation( ( Ra::Core::Vector3 { -2.0, 0.0, 0.0 } ) ) ) );
    }

    //////////////////////////////////////// create texture 2 //////////////////////////////////
    constexpr int tWidth2  = 20;
    constexpr int tHeight2 = 20;
    constexpr int tSize2   = tWidth2 * tHeight2 * 3;
    {
        unsigned char data2[tSize2];
        for ( int i = 0; i < tWidth2; ++i ) {
            for ( int j = 0; j < tHeight2; j++ ) {
                unsigned char color = ( ( i + j ) % 2 );
                data2[( i * tHeight2 + j ) * 3] =
                    ( color * int( (double)i / tWidth2 * 255 ) ) % 255;
                data2[( i * tHeight2 + j ) * 3 + 1] =
                    ( color * int( (double)j / tHeight2 * 255 ) ) % 255;
                data2[( i * tHeight2 + j ) * 3 + 2] = color * 255 % 255;
            }
        }
        auto& textureParameters2 =
            app.m_engine->getTextureManager()->addTexture( "myTexture2", tWidth2, tHeight2, data2 );
        textureParameters2.wrapS     = gl::GLenum::GL_CLAMP_TO_BORDER;
        textureParameters2.wrapT     = gl::GLenum::GL_CLAMP_TO_BORDER;
        textureParameters2.magFilter = gl::GLenum::GL_NEAREST;

        Ra::Core::Asset::BlinnPhongMaterialData matData2( "myMaterialData2" );
        // uncomment this to remove glossy highlight
        matData2.m_specular      = Ra::Core::Utils::Color::Black();
        matData2.m_hasSpecular   = false;
        matData2.m_hasTexDiffuse = true;
        matData2.m_texDiffuse    = "myTexture2";

        //! [Creating a quad geometry with texture coordinates]
        auto quad2 = Ra::Core::Geometry::makeZNormalQuad( { 1_ra, 1_ra }, {}, true );
        //! [Creating a quad geometry with texture coordinates]

        auto e2 = app.m_engine->getEntityManager()->createEntity( "Textured quad 2" );
        // the entity get's this new component ownership. a bit wired since hidden in ctor.
        auto c2 = new Ra::Engine::Scene::TriangleMeshComponent(
            "Quad Mesh 2", e2, std::move( quad2 ), &matData2 );
        ro2 = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
            c2->m_renderObjects[0] );
        ro2->setLocalTransform( Ra::Core::Transform(
            Ra::Core::Translation( ( Ra::Core::Vector3 { 1.0, 0.0, 0.0 } ) ) ) );

        //! [create the quad material]
    }

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    auto& renderTechnique = *ro2->getRenderTechnique();
    Ra::Engine::Data::ShaderConfiguration shaderConfig( "myShader" );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_VERTEX, _vertexShaderSource );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_FRAGMENT, _fragmentShaderSource );
    renderTechnique.setConfiguration( shaderConfig );

    ////////////////////////////////// create image /////////////////////////////////////////////

    int nChannel = 1;
    //    int baseSize = 1;
    Ra::Core::Asset::ImageSpec imgSpec( tWidth, tHeight, nChannel, Ra::Core::Asset::TypeUInt8 );
    auto image = std::make_shared<Ra::Core::Asset::Image>( imgSpec, nullptr, tSize );
    //    Ra::Core::Asset::ImageManager::addTexture( "proceduralImage", image );

    Ra::Core::Asset::ImageSpec imgSpec2( tWidth2, tHeight2, 3, Ra::Core::Asset::TypeUInt8 );
    auto image2 = std::make_shared<Ra::Core::Asset::Image>( imgSpec2, nullptr, tSize2 );

//    std::string imageFilename = "myFile";
//    auto image2               = std::make_shared<Ra::Core::Asset::Image>( imageFilename );
    //    Ra::Core::Asset::ImageManager::addTexture( "fileImage", image2 );

    app.m_mainWindow->getViewer()->makeCurrent();
    Ra::Engine::Data::TextureParameters textureParameters;
    textureParameters.name = "myTexture";
    auto texture3 = app.m_engine->getTextureManager()->getOrLoadTexture( textureParameters );
    app.m_mainWindow->getViewer()->doneCurrent();
    texture3->attachImage( image );

    //////////////////////////////////// create routine ////////////////////////////////////////////

    std::promise<void> exitSignal;
    std::future<void> futureObj = exitSignal.get_future();
    auto thread = std::thread( [&image, &ro, futureObj = std::move( futureObj )]() {
        const auto fps = 20;
        const auto rps = Ra::Core::Math::Pi / 2_ra; // radian per second

        int dec = 0;
        while ( futureObj.wait_for( std::chrono::milliseconds( 1 ) ) ==
                std::future_status::timeout ) {

            const auto start = std::chrono::high_resolution_clock::now();

            unsigned char newData[tSize];
            for ( int i = 0; i < tWidth; ++i ) {
                for ( int j = 0; j < tHeight; j++ ) {
                    newData[( i * tHeight + j )] =
                        (unsigned char)( 200.0 * std::abs( std::sin( float( dec ) / 4_ra ) ) *
                                         std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                                   std::cos( j * i * M_PI / 96.0 ) ) );
                    //                    newData[i * tHeight + j] = ( i + dec ) % 256;
                }
            }

            image->update( newData, tSize );

//            Ra::Core::Transform transform = Ra::Core::Transform::Identity();
//            transform.translate( Ra::Core::Vector3( -2.0, 0.0, 0.0 ) );
//            transform.rotate(
//                Eigen::AngleAxis( dec * rps / 20_ra, Ra::Core::Vector3( 0.0, 0.0, 1.0 ) ) );
//            ro->setLocalTransform( transform );

//            std::cout << "update data with dec = " << dec << std::endl;
            const auto end = start + std::chrono::microseconds( 1'000'000 / fps );
            std::this_thread::sleep_until( end );
            ++dec;
        }
    } );

    std::promise<void> exitSignal2;
    std::future<void> futureObj2 = exitSignal2.get_future();
    auto thread2 = std::thread( [&image2, &ro2, futureObj2 = std::move( futureObj2 )]() {
        const auto fps = 20;

        int dec = 0;
        while ( futureObj2.wait_for( std::chrono::milliseconds( 1 ) ) ==
                std::future_status::timeout ) {

            const auto start = std::chrono::high_resolution_clock::now();

            const auto side = dec % 100;
            const auto size = side * side * 3;

            unsigned char newData[size];
            for ( int i = 0; i < side; ++i ) {
                for ( int j = 0; j < side; j++ ) {
                    unsigned char color = ( ( i + j ) % 2 );
                    newData[( i * side + j ) * 3] =
                        ( color * int( (double)i / side * 255 ) ) % 255;
                    newData[( i * side + j ) * 3 + 1] =
                        ( color * int( (double)j / side * 255 ) ) % 255;
                    newData[( i * side + j ) * 3 + 2] = color * 255 % 255;
                }
            }

//            image2->update( newData, tSize2 );
            image2->resize( side, side, newData, size);

            //            Ra::Core::Transform transform = Ra::Core::Transform::Identity();
            //            transform.translate( Ra::Core::Vector3( -2.0, 0.0, 0.0 ) );
            //            transform.rotate( Eigen::AngleAxis( dec * rps / 20_ra,
            //                                                Ra::Core::Vector3( 0.0, 0.0, 1.0 ) )
            //                                                );
            //            ro->setLocalTransform( transform );

//            std::cout << "update data with dec = " << dec << std::endl;
            const auto end = start + std::chrono::microseconds( 1'000'000 / fps );
            std::this_thread::sleep_until( end );
            ++dec;
        }
    } );

//     terminate the app after 4 second (approximatively). Camera can be moved using mouse moves.
//        auto close_timer = new QTimer( &app );
//        close_timer->setInterval( 2000 );
//        QObject::connect( close_timer, &QTimer::timeout, [&app, &thread, &thread2, &exitSignal, &exitSignal2]() {
//            exitSignal.set_value();
//            exitSignal2.set_value();
//            thread.join();
//            thread2.join();
//            app.appNeedsToQuit();
//        } );
//        close_timer->start();

    return app.exec();
}
