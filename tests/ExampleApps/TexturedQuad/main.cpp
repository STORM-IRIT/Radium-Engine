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
#include <memory>
#include <thread>

constexpr int nQuad = 5;
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

// static Ra::Core::Asset::BlinnPhongMaterialData* g_matData = nullptr;

class QuadLife
{
  public:
    void born() {
        assert( m_app != nullptr );

        // remove glossy highlight
        m_matData.m_specular    = Ra::Core::Utils::Color::Black();
        m_matData.m_hasSpecular = true;

        //! [Creating a quad geometry with texture coordinates]
        auto quad = Ra::Core::Geometry::makeZNormalQuad( { 1_ra, 1_ra }, {}, true );
        //! [Creating a quad geometry with texture coordinates]

        //! [Create an entity and component to draw or data]
        auto e = m_app->m_engine->getEntityManager()->createEntity(
            "Textured quad " + std::to_string( m_i ) + " " + std::to_string( m_j ) );

        // the entity get's this new component ownership. a bit wired since hidden in ctor.
        auto c = new Ra::Engine::Scene::TriangleMeshComponent(
            "Quad Mesh", e, std::move( quad ), &m_matData );
        //! [Create an entity and component to draw or data]

        m_ro = Ra::Engine::RadiumEngine::getInstance()->getRenderObjectManager()->getRenderObject(
            c->m_renderObjects[0] );
        // Local transform
        m_worldTransform.translate( Ra::Core::Vector3( m_j, nQuad - m_i, 0.0 ) );
        Ra::Core::Vector3 vecScale( 0.4, 0.4, 1.0 );
        m_localTransform.scale( vecScale );
        m_ro->setLocalTransform( m_worldTransform * m_localTransform );
        //        m_ro->setLocalTransform( Ra::Core::Transform(
        //            Ra::Core::Translation( ( Ra::Core::Vector3 { m_i, m_j, 0.0 } ) ) ) );
    }
    void live() {
        assert( m_app != nullptr );
        //        return;
        //            m_thread = std::thread( [m_futureObj = std::move( m_futureObj )]() {
        m_thread = std::thread( [this]() {
            while ( m_futureObj.wait_for( std::chrono::milliseconds( 1 ) ) ==
                    std::future_status::timeout ) {

                const auto start = std::chrono::high_resolution_clock::now();
                if ( m_routine != nullptr ) { m_routine( *this ); }
                const auto end =
                    start + std::chrono::microseconds( 1'000'000 / m_routinePerSecond );
                std::this_thread::sleep_until( end );

                ++m_age;
            }
        } );
    }
    void die() {
        assert( m_app != nullptr );
        //        return;

        m_exitSignal.set_value();
        m_thread.join();
    }

  public:
    //    int size = 0;
    int m_i;
    int m_j;

    int m_width  = 0;
    int m_height = 0;
    int m_size   = 0;

    Ra::Gui::BaseApplication* m_app = nullptr;

    int m_routinePerSecond = 10;
    int m_age              = 0; // nb routine done

    Ra::Core::Transform m_worldTransform = Ra::Core::Transform::Identity();
    Ra::Core::Transform m_localTransform = Ra::Core::Transform::Identity();
    //  private:
    std::shared_ptr<Ra::Engine::Rendering::RenderObject> m_ro;
    Ra::Core::Asset::BlinnPhongMaterialData m_matData;
    std::promise<void> m_exitSignal;
    std::future<void> m_futureObj = m_exitSignal.get_future();
    std::thread m_thread;
    std::function<void( QuadLife& quadLife )> m_routine;
    std::vector<std::string> m_messages = { "auau", "ouou" };
};

void printQuadMessages( const QuadLife* quads, int width, int height );

int main( int argc, char* argv[] ) {
    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    ///////////////////////////////// Creating engine textures /////////////////////////////////////

    constexpr int proceduralEngineTexWidth  = 192;
    constexpr int proceduralEngineTexHeight = 512;
    constexpr int proceduralEngineTexSize   = proceduralEngineTexWidth * proceduralEngineTexHeight;
    {
        unsigned char data[proceduralEngineTexSize];
        // fill with some function
        for ( int i = 0; i < proceduralEngineTexWidth; ++i ) {
            for ( int j = 0; j < proceduralEngineTexHeight; j++ ) {
                data[( i * proceduralEngineTexHeight + j )] =
                    (unsigned char)( 255.0 * std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                                       std::cos( j * i * M_PI / 96.0 ) ) );
            }
        }
        auto& textureParameters = app.m_engine->getTextureManager()->addTexture(
            "proceduralEngineTex", proceduralEngineTexWidth, proceduralEngineTexHeight, data );
        // these values will be used when engine initialize texture GL representation.
        textureParameters.format         = gl::GLenum::GL_RED;
        textureParameters.internalFormat = gl::GLenum::GL_R8;
    }

    int checkerboardEngineTexSide = 10;
    int checkerboardEngineTexSize = checkerboardEngineTexSide * checkerboardEngineTexSide * 3;
    {
        unsigned char data2[checkerboardEngineTexSize];
        //        unsigned char * data2 = new unsigned char[checkerboardEngineTexSize];
        for ( int i = 0; i < checkerboardEngineTexSide; ++i ) {
            for ( int j = 0; j < checkerboardEngineTexSide; j++ ) {
                unsigned char color = ( ( i + j ) % 2 );
                data2[( i * checkerboardEngineTexSide + j ) * 3] =
                    ( color * int( (double)i / checkerboardEngineTexSide * 255 ) ) % 255;
                data2[( i * checkerboardEngineTexSide + j ) * 3 + 1] =
                    ( color * int( (double)j / checkerboardEngineTexSide * 255 ) ) % 255;
                data2[( i * checkerboardEngineTexSide + j ) * 3 + 2] = color * 255 % 255;
            }
        }
        //        unsigned char data3[checkerboardEngineTexSize]; // not worked, dangling pointer
        //        unsigned char data4[checkerboardEngineTexSize]; // not worked, dangling pointer
        //        unsigned char data5[checkerboardEngineTexSize]; // not worked, dangling pointer
        //        unsigned char data6[checkerboardEngineTexSize]; // not worked, dangling pointer
        unsigned char* data3  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data4  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data5  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data6  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data7  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data8  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data9  = new unsigned char[checkerboardEngineTexSize];
        unsigned char* data10 = new unsigned char[checkerboardEngineTexSize];
        memcpy( data3, data2, checkerboardEngineTexSize );
        memcpy( data4, data2, checkerboardEngineTexSize );
        memcpy( data5, data2, checkerboardEngineTexSize );
        memcpy( data6, data2, checkerboardEngineTexSize );
        memcpy( data7, data2, checkerboardEngineTexSize );
        memcpy( data8, data2, checkerboardEngineTexSize );
        memcpy( data9, data2, checkerboardEngineTexSize );
        memcpy( data10, data2, checkerboardEngineTexSize );

        auto& textureParameters2 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex", checkerboardEngineTexSide, checkerboardEngineTexSide, data2 );
        //        textureParameters2.format         = gl::GLenum::GL_RGB8;
        textureParameters2.wrapS     = gl::GLenum::GL_CLAMP_TO_BORDER;
        textureParameters2.wrapT     = gl::GLenum::GL_CLAMP_TO_BORDER;
        textureParameters2.magFilter = gl::GLenum::GL_NEAREST;

        auto& textureParameters3 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_minLinear_magLinear",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data3 );
        textureParameters3.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters3.magFilter = gl::GLenum::GL_LINEAR;

        auto& textureParameters4 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_minLinear_magNearest",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data4 );
        textureParameters4.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters4.magFilter = gl::GLenum::GL_NEAREST;

        auto& textureParameters5 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_magNearest_minLinear",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data5 );
        textureParameters5.minFilter = gl::GLenum::GL_NEAREST;
        textureParameters5.magFilter = gl::GLenum::GL_LINEAR;

        auto& textureParameters6 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_magNearest_minNearest",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data6 );
        textureParameters6.minFilter = gl::GLenum::GL_NEAREST;
        textureParameters6.magFilter = gl::GLenum::GL_NEAREST;

        auto& textureParameters7 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_minLinear_clampToBorder",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data7 );
        textureParameters7.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters7.wrapS     = gl::GLenum::GL_CLAMP_TO_BORDER;
        textureParameters7.wrapT     = gl::GLenum::GL_CLAMP_TO_BORDER;

        auto& textureParameters8 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_minLinear_clampToEdge",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data8 );
        textureParameters8.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters8.wrapS     = gl::GLenum::GL_CLAMP_TO_EDGE;
        textureParameters8.wrapT     = gl::GLenum::GL_CLAMP_TO_EDGE;

        auto& textureParameters9 =
            app.m_engine->getTextureManager()->addTexture( "checkerboardEngineTex_minLinear_repeat",
                                                           checkerboardEngineTexSide,
                                                           checkerboardEngineTexSide,
                                                           data9 );
        textureParameters9.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters9.wrapS     = gl::GLenum::GL_REPEAT;
        textureParameters9.wrapT     = gl::GLenum::GL_REPEAT;

        auto& textureParameters10 = app.m_engine->getTextureManager()->addTexture(
            "checkerboardEngineTex_minLinear_mirroredRepeat",
            checkerboardEngineTexSide,
            checkerboardEngineTexSide,
            data10 );
        textureParameters10.minFilter = gl::GLenum::GL_LINEAR;
        textureParameters10.wrapS     = gl::GLenum::GL_MIRRORED_REPEAT;
        textureParameters10.wrapT     = gl::GLenum::GL_MIRRORED_REPEAT;
    }

    ///////////////////////////////// Creating quads /////////////////////////////////////

    QuadLife quads[nQuad][nQuad];

    quads[0][0].m_matData.m_hasTexDiffuse = true;
    quads[0][0].m_matData.m_texDiffuse    = "proceduralEngineTex";
    //    quads[0][0].m_messages                = { "procedural engine", "texture" };
    quads[0][0].m_messages = { "a" };

    quads[0][1].m_matData.m_hasTexDiffuse = true;
    quads[0][1].m_matData.m_texDiffuse    = "checkerboardEngineTex";
    //    quads[1][1].m_messages                = { "checkerboard engine", "texture" };
    quads[0][1].m_messages = { "b" };

    quads[1][1].m_matData.m_hasTexDiffuse = true;
    quads[1][1].m_matData.m_texDiffuse    = "checkerboardEngineTex";
    //    quads[1][1].m_messages                = { "checkerboard engine", "texture" };
    quads[1][1].m_messages = { "c" };

    quads[0][nQuad - 1].m_matData.m_hasTexDiffuse = true;
    quads[0][nQuad - 1].m_matData.m_texDiffuse    = "proceduralEngineTex";
    quads[0][nQuad - 1].m_messages                = { "procedural engine", "texture" };

    quads[1][nQuad - 1].m_matData.m_hasTexDiffuse = true;
    quads[1][nQuad - 1].m_matData.m_texDiffuse    = "checkerboardEngineTex";
    quads[1][nQuad - 1].m_messages                = { "checkerboard engine", "texture" };

    quads[0][nQuad - 2].m_matData.m_hasTexDiffuse = true;
    quads[0][nQuad - 2].m_matData.m_texDiffuse    = "checkerboardEngineTex_minLinear_magLinear";
    quads[0][nQuad - 2].m_messages = { "checkerboard engine", "texture", "minLinear + magLinear" };

    quads[1][nQuad - 2].m_matData.m_hasTexDiffuse = true;
    quads[1][nQuad - 2].m_matData.m_texDiffuse    = "checkerboardEngineTex_minLinear_magNearest";
    quads[1][nQuad - 2].m_messages = { "checkerboard engine", "texture", "minLinear + magNearest" };

    quads[2][nQuad - 2].m_matData.m_hasTexDiffuse = true;
    quads[2][nQuad - 2].m_matData.m_texDiffuse    = "checkerboardEngineTex_minNearest_magLinear";
    quads[2][nQuad - 2].m_messages = { "checkerboard engine", "texture", "minNearest + magLinear" };

    quads[3][nQuad - 2].m_matData.m_hasTexDiffuse = true;
    quads[3][nQuad - 2].m_matData.m_texDiffuse    = "checkerboardEngineTex_minNearest_magNearest";
    quads[3][nQuad - 2].m_messages                = {
        "checkerboard engine", "texture", "minNearest + magNearest" };

    quads[0][nQuad - 3].m_matData.m_hasTexDiffuse = true;
    quads[0][nQuad - 3].m_matData.m_texDiffuse    = "checkerboardEngineTex_minLinear_clampToBorder";
    quads[0][nQuad - 3].m_messages                = {
        "checkerboard engine", "texture", "minLinear + clampToBorder" };

    quads[1][nQuad - 3].m_matData.m_hasTexDiffuse = true;
    quads[1][nQuad - 3].m_matData.m_texDiffuse    = "checkerboardEngineTex_minLinear_clampToEdge";
    quads[1][nQuad - 3].m_messages                = {
        "checkerboard engine", "texture", "minLinear + clampToEdge" };

    quads[2][nQuad - 3].m_matData.m_hasTexDiffuse = true;
    quads[2][nQuad - 3].m_matData.m_texDiffuse    = "checkerboardEngineTex_minLinear_repeat";
    quads[2][nQuad - 3].m_messages = { "checkerboard engine", "texture", "minLinear + repeat" };

    quads[3][nQuad - 3].m_matData.m_hasTexDiffuse = true;
    quads[3][nQuad - 3].m_matData.m_texDiffuse = "checkerboardEngineTex_minLinear_mirroredRepeat";
    quads[3][nQuad - 3].m_messages             = {
        "checkerboard engine", "texture", "minLinear + mirroredRepeat" };

    // init geometry
    for ( int i = 0; i < nQuad; ++i ) {
        for ( int j = 0; j < nQuad; ++j ) {
            QuadLife& quadLife = quads[i][j];
            quadLife.m_app     = &app;
            quadLife.m_i       = i;
            quadLife.m_j       = j;
            quadLife.born();
        }
    }
    //    auto blinnPhongMaterial =
    //        std::make_shared<Ra::Engine::Data::BlinnPhongMaterial>( "Shaded Material" );
    //    blinnPhongMaterial->m_perVertexColor = true;
    //    blinnPhongMaterial->m_ks             = Ra::Engine::Data::Color::White();
    //    blinnPhongMaterial->m_ns             = 100_ra;

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    ////////////////////////////////// Create images ///////////////////////////////////

    Ra::Core::Asset::ImageSpec imgSpec(
        proceduralEngineTexWidth, proceduralEngineTexHeight, 1, Ra::Core::Asset::TypeUInt8 );
    auto proceduralImage =
        std::make_shared<Ra::Core::Asset::Image>( imgSpec, nullptr, proceduralEngineTexSize );

    Ra::Core::Asset::ImageSpec imgSpec2(
        checkerboardEngineTexSide, checkerboardEngineTexSide, 3, Ra::Core::Asset::TypeUInt8 );
    auto checkerboardImage =
        std::make_shared<Ra::Core::Asset::Image>( imgSpec2, nullptr, checkerboardEngineTexSize );

    ////////////////////////////////// Get engine textures ///////////////////////////////////

    app.m_mainWindow->getViewer()->makeCurrent();
    Ra::Engine::Data::TextureParameters textureParameters;
    textureParameters.name = "proceduralEngineTex";
    auto proceduralEngineTex =
        app.m_engine->getTextureManager()->getOrLoadTexture( textureParameters );
    textureParameters.name = "checkerboardEngineTex";
    auto checkerboardEngineTex =
        app.m_engine->getTextureManager()->getOrLoadTexture( textureParameters );
    app.m_mainWindow->getViewer()->doneCurrent();

    Ra::Engine::Data::ShaderConfiguration shaderConfig( "myShader" );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_VERTEX, _vertexShaderSource );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_FRAGMENT, _fragmentShaderSource );

    for ( int i = 0; i < nQuad; ++i ) {
        for ( int j = 0; j < nQuad; ++j ) {
            quads[i][j].m_ro->getRenderTechnique()->setConfiguration( shaderConfig );
        }
    }

    //////////////////////////////////// Create routines ////////////////////////////////////////

    quads[0][0].m_routine = [&proceduralImage]( QuadLife& quadLife ) {
        constexpr auto rps = Ra::Core::Math::Pi / 2_ra; // radian per second

        unsigned char newData[proceduralEngineTexSize];
        for ( int i = 0; i < proceduralEngineTexWidth; ++i ) {
            for ( int j = 0; j < proceduralEngineTexHeight; j++ ) {
                newData[( i * proceduralEngineTexHeight + j )] =
                    (unsigned char)( 200.0 *
                                     std::abs( std::sin( float( quadLife.m_age ) / 4_ra ) ) *
                                     std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                               std::cos( j * i * M_PI / 96.0 ) ) );
            }
        }

        proceduralImage->update( newData, proceduralEngineTexSize );
        std::memset( newData, 255, proceduralEngineTexSize );

        Ra::Core::Transform rotateTransform = Ra::Core::Transform::Identity();
        rotateTransform.rotate(
            Eigen::AngleAxis( quadLife.m_age * rps / quadLife.m_routinePerSecond,
                              Ra::Core::Vector3( 0.0, 0.0, 1.0 ) ) );

        quadLife.m_ro->setLocalTransform( quadLife.m_worldTransform * quadLife.m_localTransform *
                                          rotateTransform );
    };
    proceduralEngineTex->attachImage( proceduralImage );
    quads[0][0].m_routinePerSecond = 60;
    //    quads[0][0].m_ro->getRenderTechnique()->setConfiguration( shaderConfig );

    quads[1][1].m_routine = [&checkerboardImage,
                             &checkerboardEngineTexSize,
                             &checkerboardEngineTexSide]( QuadLife& quadLife ) {
        //        checkerboardEngineTexSide = quadLife.m_age % 100;
        checkerboardEngineTexSize = checkerboardEngineTexSide * checkerboardEngineTexSide * 3;

        unsigned char newData[checkerboardEngineTexSize];
        for ( int i = 0; i < checkerboardEngineTexSide; ++i ) {
            for ( int j = 0; j < checkerboardEngineTexSide; j++ ) {
                unsigned char color = ( ( i + j ) % 2 );
                newData[( i * checkerboardEngineTexSide + j ) * 3] =
                    ( color * int( (double)i / checkerboardEngineTexSide * 255 ) ) % 255;
                newData[( i * checkerboardEngineTexSide + j ) * 3 + 1] =
                    ( color * int( (double)j / checkerboardEngineTexSide * 255 ) ) % 255;
                newData[( i * checkerboardEngineTexSide + j ) * 3 + 2] = color * 255 % 255;
            }
        }

        //            image2->update( newData, tSize2 );
        checkerboardImage->update( newData, checkerboardEngineTexSize );
        //        checkerboardImage->resize( checkerboardEngineTexSide,
        //                                   checkerboardEngineTexSide,
        //                                   newData,
        //                                   checkerboardEngineTexSize );
    };
    //    quads[1][1].m_ro->getRenderTechnique()->setConfiguration( shaderConfig );
    //    checkerboardEngineTex->attachImage( checkerboardImage );

    //////////////////////////////////// Starting routines ////////////////////////////////////////

    // Maternity, starting quad lifes
    for ( int i = 0; i < nQuad; ++i ) {
        for ( int j = 0; j < nQuad; ++j ) {
            QuadLife& quadLife = quads[i][j];
            quadLife.live();
        }
    }

    //    terminate the app after 4 second( approximatively ).Camera can be moved using mouse moves.
    auto close_timer = new QTimer( &app );
    close_timer->setInterval( 4000 );
    QObject::connect( close_timer, &QTimer::timeout, [&app, &quads]() {
        for ( int i = 0; i < nQuad; ++i ) {
            for ( int j = 0; j < nQuad; ++j ) {
                QuadLife& quadLife = quads[i][j];
                quadLife.die();
            }
        }
        app.appNeedsToQuit();
    } );
    close_timer->start();

    printQuadMessages( (const QuadLife*)quads, nQuad, nQuad );

    return app.exec();
}

void printQuadMessages( const QuadLife* quads, int width, int height ) {

    constexpr int hSpace = 5;
    constexpr int vSpace = hSpace / 3;

    constexpr int hSide = 30;
    constexpr int vSide = hSide / 3;

    // for each quad line
    for ( int i = 0; i < height; ++i ) {
        // top bar
        for ( int j = 0; j < height; ++j ) {
            for ( int s = 0; s < hSide; ++s ) {
                std::cout << "-";
            }
            for ( int s = 0; s < hSpace; ++s ) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;

        // for each line in quad
        for ( int j = 0; j < vSide; ++j ) {

            // for each quad column
            for ( int k = 0; k < width; ++k ) {
                std::cout << "|";

                //                const auto& messages = quads[(height - i - 1) * width + (width - k
                //                - 1)].m_messages;
                const auto& messages = quads[i * width + k].m_messages;
                assert( messages.size() < vSide + 1 );
                int nbMessage = messages.size();

                int cur = j - vSide / 2 + nbMessage / 2;

                if ( 0 <= cur && cur < nbMessage ) {
                    const auto& message = messages.at( cur );
                    assert( message.size() < hSide - 2 );
                    const auto lenMessage = message.size();
                    for ( int l = 0; l < ( hSide - lenMessage ) / 2 - 1; ++l ) {
                        std::cout << " ";
                    }
                    std::cout << message;
                    for ( int l = 0; l < ( hSide - lenMessage - 1 ) / 2; ++l ) {
                        std::cout << " ";
                    }
                }
                else {
                    for ( int l = 1; l < hSide - 1; ++l ) {
                        std::cout << " ";
                    }
                }
                std::cout << "|";

                // h margin
                for ( int s = 0; s < hSpace; ++s ) {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }

        // bottom bar
        for ( int j = 0; j < height; ++j ) {
            for ( int s = 0; s < hSide; ++s ) {
                std::cout << "-";
            }
            for ( int s = 0; s < hSpace; ++s ) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;

        // v margin
        for ( int s = 0; s < vSpace; ++s ) {
            std::cout << std::endl;
        }
    }
}
