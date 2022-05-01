// Include Radium base application and its simple Gui
#include <Core/Resources/Resources.hpp>
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
//#include <Engine/OpenGL.hpp>

#include <QTimer>
#include <algorithm>
#include <cmath>
#include <future>
#include <memory>
#include <thread>

constexpr int nQuad = 5; // side

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
    "   vec3 color = material.kd.rgb;\n"
    "   if (material.tex.hasKd == 1) { color =  texture(material.tex.kd, in_texcoord.xy).rgb; }\n"
    "   out_color =  vec4(color, 1.0);\n"
    "   // out_color =  ( 1 + cos( 20 * ( in_pos.x + aScalarUniform ) ) ) * 0.5 * aColorUniform;\n"
    "}\n" };

struct QuadLife {
    void born() {
        assert( m_app != nullptr );

        // remove glossy highlight
        m_matData.m_specular    = Ra::Core::Utils::Color::Black();
        m_matData.m_hasSpecular = true;
        // add default diffuse color if user didn't set one
        if ( !m_matData.m_hasDiffuse ) {
            m_matData.m_diffuse    = Ra::Core::Utils::Color::Black();
            m_matData.m_hasDiffuse = true;
        }

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

        m_worldTransform.translate( Ra::Core::Vector3( m_j, nQuad - m_i, 0.0 ) );
        Ra::Core::Vector3 vecScale( 0.4, 0.4, 1.0 );
        m_localTransform.scale( vecScale );
        m_ro->setLocalTransform( m_worldTransform * m_localTransform );

        // get message from engine texture name
        if ( !m_matData.m_texDiffuse.empty() ) {
            std::string s         = m_matData.m_texDiffuse;
            std::string delimiter = "_";

            size_t pos = 0;
            std::string token;
            while ( ( pos = s.find( delimiter ) ) != std::string::npos ) {
                token = s.substr( 0, pos );
                s.erase( 0, pos + delimiter.length() );
                m_messages.push_back( token );
            }
            m_messages.push_back( s );
        }
    }

    void live() {
        assert( m_app != nullptr );

        if ( m_routine != nullptr ) {
            char str[32];
            sprintf( str, "%.1f", (float)m_routinePerSecond );
            m_messages.push_back( std::string( str ) + "Hz" );

            m_thread = std::thread( [this]() {
                while ( m_futureObj.wait_for( std::chrono::milliseconds( 1 ) ) ==
                        std::future_status::timeout ) {

                    const auto start = std::chrono::high_resolution_clock::now();
                    if ( m_routine != nullptr ) { m_routine( *this ); }
                    const auto end = start + std::chrono::microseconds(
                                                 (int)( 1'000'000 / m_routinePerSecond ) );
                    std::this_thread::sleep_until( end );

                    ++m_age;
                }
            } );
        }
    }

    void die() {
        assert( m_app != nullptr );

        if ( m_routine != nullptr ) {
            m_exitSignal.set_value();
            m_thread.join();
        }
    }

    int m_i;
    int m_j;

    Ra::Gui::BaseApplication* m_app = nullptr;

    Scalar m_routinePerSecond = 10_ra;
    int m_age                 = 0; // nb routine done

    Ra::Core::Transform m_worldTransform = Ra::Core::Transform::Identity();
    Ra::Core::Transform m_localTransform = Ra::Core::Transform::Identity();
    std::shared_ptr<Ra::Engine::Rendering::RenderObject> m_ro;
    Ra::Core::Asset::BlinnPhongMaterialData m_matData;
    std::promise<void> m_exitSignal;
    std::future<void> m_futureObj = m_exitSignal.get_future();
    std::thread m_thread;
    std::function<void( QuadLife& quadLife )> m_routine;
    std::vector<std::string> m_messages;
};

enum EngineTextureInstance : uint {
    PROCEDURAL = 0,
    CHECKER_BOARD,
    CHECKER_BOARD_MIN_LINEAR_MAG_NEAREST,
    CHECKER_BOARD_MIN_NEAREST_MAG_LINEAR,
    CHECKER_BOARD_MIN_NEAREST_MAG_NEAREST,
    CHECKER_BOARD_WRAP_CLAMP_TO_BORDER,
    CHECKER_BOARD_WRAP_REPEAT,
    CHECKER_BOARD_WRAP_MIRRORED_REPEAT,
    SHIFTER,
    ATTACH_DETACH,
    GRADIENT,
    RADIUM_LOGO_PNG,
    RADIUM_LOGO_JPG,
    BLINK,

    COUNT,
    NONE,
};

// clang-format off
const EngineTextureInstance g_initQuadTexture[nQuad][nQuad] {
    { PROCEDURAL, 							CHECKER_BOARD, 							NONE, 									NONE, 			NONE },
    { CHECKER_BOARD_MIN_LINEAR_MAG_NEAREST, CHECKER_BOARD_MIN_NEAREST_MAG_LINEAR, 	CHECKER_BOARD_MIN_NEAREST_MAG_NEAREST, 	NONE, 			NONE },
    { CHECKER_BOARD_WRAP_CLAMP_TO_BORDER, 	CHECKER_BOARD_WRAP_REPEAT, 				CHECKER_BOARD_WRAP_MIRRORED_REPEAT, 	NONE, 			BLINK },
    { RADIUM_LOGO_PNG, 						RADIUM_LOGO_JPG, 						NONE, 									NONE, 			GRADIENT },
    { PROCEDURAL, 							CHECKER_BOARD, 							SHIFTER, 								ATTACH_DETACH, 	NONE /* white color */ }
};
// clang-format on

using namespace gl;

// clang-format off
const Ra::Engine::Data::TextureParameters g_initTextureParameters[EngineTextureInstance::COUNT] {
  { "procedural", GL_TEXTURE_2D, 192, 512, 1, GL_RED, GL_R8, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "checkerboard", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "checkerboard_minLinear_magNearest", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_NEAREST, nullptr },
  { "checkerboard_minNearest_magLinear", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_LINEAR, nullptr },
  { "checkerboard_minNearest_magNearest", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, nullptr },
  { "checkerboard_wrapClampToBorder", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR, nullptr },
  { "checkerboard_wrapRepeat", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_REPEAT, GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, nullptr },
  { "checkerboard_wrapMirroredRepeat", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_MIRRORED_REPEAT, GL_LINEAR, GL_LINEAR, nullptr },
  { "shifter", GL_TEXTURE_2D, 100, 100, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "attach/detach", GL_TEXTURE_2D, 10, 10, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "gradient", GL_TEXTURE_2D, 256, 256, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "radium-logo.png", GL_TEXTURE_2D, 512, 512, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "radium-logo.jpg", GL_TEXTURE_2D, 512, 512, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
  { "blink_white/black", GL_TEXTURE_2D, 100, 100, 1, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, nullptr },
};
// clang-format on

constexpr size_t g_engineTextureSizes[EngineTextureInstance::COUNT] {
    192 * 512 * 1,
    10 * 10 * 3,
    10 * 10 * 3,
    10 * 10 * 3,
    10 * 10 * 3,
    10 * 10 * 3,
    10 * 10 * 3,
    10 * 10 * 3,
    100 * 100 * 3,
    10 * 10 * 3,
    256 * 256 * 3,
    512 * 512 * 3,
    512 * 512 * 3,
    100 * 100 * 3,
};

struct EngineTexture {
    Ra::Engine::Data::TextureParameters m_initTextureParameters;

    unsigned char* m_data                = nullptr;
    int nChannel                         = 0;
    size_t m_sizeData                    = 0;
    Ra::Engine::Data::Texture* m_texture = nullptr;

    void setData( void* data, size_t len ) {
        assert( m_initTextureParameters.type == GL_UNSIGNED_BYTE );
        switch ( m_initTextureParameters.format ) {
        case GL_RGB:
            nChannel = 3;
            break;
        case GL_RED:
            nChannel = 1;
            break;
        default:
            assert( false );
        }
        m_sizeData = m_initTextureParameters.width * m_initTextureParameters.height * nChannel;
        assert( m_sizeData == len );

        assert( m_data == nullptr );
        assert( data != nullptr );
        m_data = new unsigned char[len];
        memcpy( m_data, data, len );
    }
    void init( Ra::Gui::BaseApplication& app ) {
        //        assert( m_data != nullptr );

        auto& textureParameters =
            app.m_engine->getTextureManager()->addTexture( m_initTextureParameters.name,
                                                           m_initTextureParameters.width,
                                                           m_initTextureParameters.height,
                                                           m_data );

        // these values will be used when engine initialize texture GL representation.
        textureParameters.format         = m_initTextureParameters.format;
        textureParameters.internalFormat = m_initTextureParameters.internalFormat;
        textureParameters.minFilter      = m_initTextureParameters.minFilter;
        textureParameters.magFilter      = m_initTextureParameters.magFilter;
        textureParameters.wrapS          = m_initTextureParameters.wrapS;
        textureParameters.wrapT          = m_initTextureParameters.wrapT;
    }

    void getTexture( Ra::Gui::BaseApplication& app ) {
        Ra::Engine::Data::TextureParameters textureParameters;
        textureParameters.name = m_initTextureParameters.name;
        m_texture = app.m_engine->getTextureManager()->getOrLoadTexture( textureParameters );
    }

    ~EngineTexture() {
        if ( m_data != nullptr ) delete[] m_data;
    }
};
static EngineTexture g_engineTextures[EngineTextureInstance::COUNT];

void printQuadMessages( const QuadLife* quads, int width, int height );

int main( int argc, char* argv[] ) {

    //! [Creating the application]
    Ra::Gui::BaseApplication app( argc, argv );
    app.initialize( Ra::Gui::SimpleWindowFactory {} );
    //! [Creating the application]

    ///////////////////////////////// Creating engine textures /////////////////////////////////////

    for ( int i = 0; i < (int)EngineTextureInstance::COUNT; ++i ) {
        g_engineTextures[i].m_initTextureParameters = g_initTextureParameters[i];
    }

    // all data set here will be read by initializeGL with dangling pointers
    {
        constexpr int width    = 192;
        constexpr int height   = 512;
        constexpr int nChannel = 1;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        for ( int i = 0; i < width; ++i ) {
            for ( int j = 0; j < height; ++j ) {
                data[( i * height + j )] =
                    (unsigned char)( 255.0 * std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                                       std::cos( j * i * M_PI / 96.0 ) ) );
            }
        }
        g_engineTextures[PROCEDURAL].setData( data, size );
    }

    {
        constexpr int width    = 10;
        constexpr int height   = 10;
        constexpr int nChannel = 3;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        for ( int i = 0; i < width; ++i ) {
            for ( int j = 0; j < height; ++j ) {
                unsigned char color          = ( ( i + j ) % 2 );
                data[( i * height + j ) * 3] = ( color * int( (double)i / height * 255 ) ) % 255;
                data[( i * height + j ) * 3 + 1] =
                    ( color * int( (double)j / height * 255 ) ) % 255;
                data[( i * height + j ) * 3 + 2] = color * 255 % 255;
            }
        }

        for ( int i = CHECKER_BOARD; i < (int)CHECKER_BOARD_WRAP_MIRRORED_REPEAT + 1; ++i ) {
            g_engineTextures[i].setData( data, size );
        }
    }

    {
        constexpr int width    = 100;
        constexpr int height   = 100;
        constexpr int nChannel = 3;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        memset( data, 0, size );
        g_engineTextures[SHIFTER].setData( data, size );
        g_engineTextures[BLINK].setData( data, size );
    }

    {
        constexpr int width    = 10;
        constexpr int height   = 10;
        constexpr int nChannel = 3;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        for ( int i = 0; i < width; ++i ) {
            for ( int j = 0; j < height; ++j ) {
                data[( i * height + j ) * 3]     = 255;
                data[( i * height + j ) * 3 + 1] = 0;
                data[( i * height + j ) * 3 + 2] = 0;
            }
        }

        g_engineTextures[ATTACH_DETACH].setData( data, size );
    }

    {
        constexpr int width    = 256;
        constexpr int height   = 256;
        constexpr int nChannel = 3;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        for ( int i = 0; i < width; ++i ) {
            for ( int j = 0; j < height; ++j ) {
                data[( i * height + j ) * 3]     = i;
                data[( i * height + j ) * 3 + 1] = i;
                data[( i * height + j ) * 3 + 2] = i;
            }
        }

        g_engineTextures[GRADIENT].setData( data, size );
    }

    {
        constexpr int width    = 512;
        constexpr int height   = 512;
        constexpr int nChannel = 3;
        constexpr int size     = width * height * nChannel;

        unsigned char data[size];
        for ( int i = 0; i < width; ++i ) {
            for ( int j = 0; j < height; ++j ) {
                data[( i * height + j ) * 3]     = 0;
                data[( i * height + j ) * 3 + 1] = 255;
                data[( i * height + j ) * 3 + 2] = 0;
            }
        }

        g_engineTextures[RADIUM_LOGO_JPG].setData( data, size );
        g_engineTextures[RADIUM_LOGO_PNG].setData( data, size );
    }

    for ( int i = 0; i < (int)EngineTextureInstance::COUNT; ++i ) {
        assert( g_engineTextures[i].m_sizeData == g_engineTextureSizes[i] );
    }
    for ( auto& engineTexture : g_engineTextures ) {
        engineTexture.init( app );
    }

    ///////////////////////////////// Creating quads /////////////////////////////////////

    QuadLife quads[nQuad][nQuad];

    for ( int i = 0; i < nQuad; ++i ) {
        for ( int j = 0; j < nQuad; ++j ) {
            auto engineTextureInstance = g_initQuadTexture[i][j];
            if ( engineTextureInstance != NONE ) {
                quads[i][j].m_matData.m_hasTexDiffuse = true;
                quads[i][j].m_matData.m_texDiffuse =
                    g_initTextureParameters[engineTextureInstance].name;
            }
        }
    }

    auto& transformTestQuad                  = quads[nQuad - 1][nQuad - 1];
    transformTestQuad.m_matData.m_diffuse    = Ra::Core::Utils::Color::White();
    transformTestQuad.m_matData.m_hasDiffuse = true;

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

    //! [Tell the window that something is to be displayed]
    app.m_mainWindow->prepareDisplay();
    //! [Tell the window that something is to be displayed]

    ////////////////////////////////// Create images ///////////////////////////////////

    std::vector<std::shared_ptr<Ra::Core::Asset::Image>> images;

    const auto& proceduralInitParameters = g_initTextureParameters[PROCEDURAL];
    Ra::Core::Asset::ImageSpec imgSpec( proceduralInitParameters.width,
                                        proceduralInitParameters.height,
                                        1,
                                        Ra::Core::Asset::TypeUInt8 );
    auto imageProcedural = std::make_shared<Ra::Core::Asset::Image>(
        imgSpec, nullptr, g_engineTextures[PROCEDURAL].m_sizeData );
    images.push_back( imageProcedural );

    const auto& checkerboardInitParameters = g_initTextureParameters[CHECKER_BOARD];
    Ra::Core::Asset::ImageSpec imgSpec2( checkerboardInitParameters.width,
                                         checkerboardInitParameters.height,
                                         3,
                                         Ra::Core::Asset::TypeUInt8 );
    auto imageCheckerboard = std::make_shared<Ra::Core::Asset::Image>(
        imgSpec2, nullptr, g_engineTextures[CHECKER_BOARD].m_sizeData );
    images.push_back( imageCheckerboard );

    const auto& grandientInitParameters = g_initTextureParameters[GRADIENT];
    Ra::Core::Asset::ImageSpec imgSpec3( grandientInitParameters.width,
                                         grandientInitParameters.height,
                                         3,
                                         Ra::Core::Asset::TypeUInt8 );
    auto imageGradient = std::make_shared<Ra::Core::Asset::Image>(
        imgSpec3, nullptr, g_engineTextures[GRADIENT].m_sizeData );
    images.push_back( imageGradient );

    auto rp                 = Ra::Core::Resources::getResourcesPath();
    std::string filename    = *rp + "/Demos/Assets/radium-logo.png";
    auto imageRadiumLogoPng = std::make_shared<Ra::Core::Asset::Image>( filename );
    images.push_back( imageRadiumLogoPng );

    std::string filename2   = *rp + "/Demos/Assets/radium-logo.jpg";
    auto imageRadiumLogoJpg = std::make_shared<Ra::Core::Asset::Image>( filename2 );
    images.push_back( imageRadiumLogoJpg );

    const auto& blinkInitParameters = g_initTextureParameters[BLINK];
    Ra::Core::Asset::ImageSpec imgSpec4(
        blinkInitParameters.width, blinkInitParameters.height, 3, Ra::Core::Asset::TypeUInt8 );
    auto imageBlink = std::make_shared<Ra::Core::Asset::Image>(
        imgSpec4, nullptr, g_engineTextures[BLINK].m_sizeData );
    images.push_back( imageBlink );

    ////////////////////////////////// Get engine textures ///////////////////////////////////

    app.m_mainWindow->getViewer()->makeCurrent();

    for ( int i = 0; i < (int)EngineTextureInstance::COUNT; ++i ) {
        g_engineTextures[i].getTexture( app );
    }

    app.m_mainWindow->getViewer()->doneCurrent();

    ////////////////////////////////// Attach images to textures /////////////////////////////

    g_engineTextures[PROCEDURAL].m_texture->attachImage( imageProcedural );
    for ( int i = CHECKER_BOARD; i < (int)CHECKER_BOARD_WRAP_MIRRORED_REPEAT + 1; ++i ) {
        g_engineTextures[i].m_texture->attachImage( imageCheckerboard );
    }
    g_engineTextures[GRADIENT].m_texture->attachImage( imageGradient );
    g_engineTextures[RADIUM_LOGO_PNG].m_texture->attachImage( imageRadiumLogoPng );
    g_engineTextures[RADIUM_LOGO_JPG].m_texture->attachImage( imageRadiumLogoJpg );
    g_engineTextures[BLINK].m_texture->attachImage( imageBlink );

    ////////////////////////////////// Set user shader ///////////////////////////////////////

    Ra::Engine::Data::ShaderConfiguration shaderConfig( "myShader" );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_VERTEX, _vertexShaderSource );
    shaderConfig.addShaderSource( Ra::Engine::Data::ShaderType_FRAGMENT, _fragmentShaderSource );

    for ( int i = 0; i < nQuad; ++i ) {
        for ( int j = 0; j < nQuad; ++j ) {
            quads[i][j].m_ro->getRenderTechnique()->setConfiguration( shaderConfig );
        }
    }

    //////////////////////////////////// Create routines /////////////////////////////////////

    auto& proceduralQuad = quads[nQuad - 1][0];
    {
        proceduralQuad.m_routine = [&imageProcedural]( QuadLife& quadLife ) {
            constexpr auto rps = Ra::Core::Math::Pi / 2_ra; // radian per second

            const auto& proceduralInitParameters = g_initTextureParameters[PROCEDURAL];
            const auto& size                     = g_engineTextureSizes[PROCEDURAL];

            unsigned char newData[size];
            for ( int i = 0; i < proceduralInitParameters.width; ++i ) {
                for ( int j = 0; j < proceduralInitParameters.height; j++ ) {
                    newData[( i * proceduralInitParameters.height + j )] =
                        (unsigned char)( 200.0 *
                                         std::abs( std::sin( float( quadLife.m_age ) / 4_ra ) ) *
                                         std::abs( std::sin( j * i * M_PI / 64.0 ) *
                                                   std::cos( j * i * M_PI / 96.0 ) ) );
                }
            }

            imageProcedural->update( newData, size );
            std::memset( newData, 255, size );

            Ra::Core::Transform rotateTransform = Ra::Core::Transform::Identity();
            rotateTransform.rotate(
                Eigen::AngleAxis( quadLife.m_age * rps / quadLife.m_routinePerSecond,
                                  Ra::Core::Vector3( 0.0, 0.0, 1.0 ) ) );

            quadLife.m_ro->setLocalTransform( quadLife.m_worldTransform *
                                              quadLife.m_localTransform * rotateTransform );
        };
        proceduralQuad.m_routinePerSecond = 60;
        proceduralQuad.m_messages.push_back( "rotate transform" );
        proceduralQuad.m_messages.push_back( "update image" );
    }

    auto& checkerboardQuad = quads[nQuad - 1][1];
    {
        checkerboardQuad.m_routine = [&imageCheckerboard]( QuadLife& quadLife ) {
            int checkerboard_engineTexSide = quadLife.m_age % 100;
            int checkerboard_engineTexSize =
                checkerboard_engineTexSide * checkerboard_engineTexSide * 3;

            auto newData = new unsigned char[checkerboard_engineTexSize];
            for ( int i = 0; i < checkerboard_engineTexSide; ++i ) {
                for ( int j = 0; j < checkerboard_engineTexSide; j++ ) {
                    unsigned char color = ( ( i + j ) % 2 );
                    newData[( i * checkerboard_engineTexSide + j ) * 3] =
                        ( color * int( (double)i / checkerboard_engineTexSide * 255 ) ) % 255;
                    newData[( i * checkerboard_engineTexSide + j ) * 3 + 1] =
                        ( color * int( (double)j / checkerboard_engineTexSide * 255 ) ) % 255;
                    newData[( i * checkerboard_engineTexSide + j ) * 3 + 2] = color * 255 % 255;
                }
            }

            imageCheckerboard->resize( checkerboard_engineTexSide,
                                       checkerboard_engineTexSide,
                                       newData,
                                       checkerboard_engineTexSize );
            delete[] newData;
        };
        checkerboardQuad.m_messages.push_back( "resize image" );
    }

    auto& gradientQuad = quads[nQuad - 2][nQuad - 1];
    {
        gradientQuad.m_routine = [&imageGradient]( QuadLife& quadLife ) {
            const auto& proceduralInitParameters = g_initTextureParameters[GRADIENT];
            const auto& size                     = g_engineTextureSizes[GRADIENT];
            const auto& side                     = proceduralInitParameters.width;

            unsigned char newData[size];
            for ( int i = 0; i < side; ++i ) {
                for ( int j = 0; j < side; j++ ) {
                    unsigned char color               = ( i + quadLife.m_age ) % 256;
                    newData[( i * side + j ) * 3]     = color;
                    newData[( i * side + j ) * 3 + 1] = color;
                    newData[( i * side + j ) * 3 + 2] = color;
                }
            }

            imageGradient->update( newData, size );
        };
        gradientQuad.m_messages.push_back( "update image" );
        gradientQuad.m_routinePerSecond = 256;
    }

    auto& textureShifterQuad = quads[nQuad - 1][2];
    {
        textureShifterQuad.m_routine = [&images]( QuadLife& quadLife ) {
            g_engineTextures[SHIFTER].m_texture->attachImage(
                images[quadLife.m_age % images.size()] );
        };
        textureShifterQuad.m_routinePerSecond = 0.5;
    }

    auto& attachDetachQuad = quads[nQuad - 1][3];
    {
        attachDetachQuad.m_routine = [&imageProcedural]( QuadLife& quadLife ) {
            if ( quadLife.m_age % 2 == 0 ) {
                g_engineTextures[ATTACH_DETACH].m_texture->attachImage( imageProcedural );
            }
            else {
                g_engineTextures[ATTACH_DETACH].m_texture->detachImage();
            }
        };
        attachDetachQuad.m_routinePerSecond = 0.5;
        attachDetachQuad.m_messages.push_back( "to procedural image" );
    }

    {
        transformTestQuad.m_routine = []( QuadLife& quadLife ) {
            constexpr auto rps                  = Ra::Core::Math::Pi / 4_ra; // radian per second
            Ra::Core::Transform rotateTransform = Ra::Core::Transform::Identity();
            rotateTransform.rotate(
                Eigen::AngleAxis( quadLife.m_age * rps / quadLife.m_routinePerSecond,
                                  Ra::Core::Vector3( 0.0, 0.0, 1.0 ) ) );

            quadLife.m_ro->setLocalTransform( quadLife.m_worldTransform *
                                              quadLife.m_localTransform * rotateTransform );
        };
        transformTestQuad.m_routinePerSecond = 120;
        transformTestQuad.m_messages.push_back( "white color" );
        transformTestQuad.m_messages.push_back( "no texture" );
        transformTestQuad.m_messages.push_back( "rotate transform" );
    }

    auto& blinkQuad = quads[nQuad - 3][nQuad - 1];
    {
        int blkps                    = 30; // blink per second
        blinkQuad.m_routinePerSecond = blkps;
        int iPeriod                  = 0;
        blinkQuad.m_routine          = [&imageBlink, &iPeriod, &blkps]( QuadLife& quadLife ) {
            const auto& size = g_engineTextureSizes[BLINK];

            unsigned char newData[size];
            std::memset( newData, ( quadLife.m_age % 2 == 0 ) ? ( 255 ) : ( 0 ), size );
            imageBlink->update( newData, size );

            ++iPeriod;
            if ( iPeriod == blkps ) {
                blkps                       = blkps % 60 + 1;
                quadLife.m_routinePerSecond = blkps;
                iPeriod                     = 0;
            }
        };
        blinkQuad.m_messages.push_back( "blinking texture" );
        blinkQuad.m_messages.push_back( "1Hz -> 60Hz" );
    }

    //////////////////////////////////// Starting routines ////////////////////////////////////////

    // Maternity : starting quad lifes
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
    int hSideMin = 0;
    int vSideMin = 0;

    for ( int i = 0; i < width; ++i ) {
        for ( int j = 0; j < height; ++j ) {
            const auto& messages = quads[i * width + j].m_messages;
            vSideMin             = std::max( vSideMin, (int)messages.size() );
            for ( const auto& message : messages ) {
                hSideMin = std::max( hSideMin, (int)message.size() );
            }
        }
    }

    constexpr int hSpace = 3;
    constexpr int vSpace = hSpace / 3;

    const int hSide = hSideMin + 4;
    const int vSide = std::max( hSide / 3, vSideMin );

    // for each quads line
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

            // for each quads column
            for ( int k = 0; k < width; ++k ) {
                std::cout << "|";

                const auto& messages = quads[i * width + k].m_messages;
                //                assert( messages.size() < vSide + 1 );
                int nbMessage = std::min( (int)messages.size(), vSide + 1 );

                int cur = j - vSide / 2 + nbMessage / 2;

                if ( 0 <= cur && cur < nbMessage ) {
                    const auto& message = messages.at( cur );
                    //                    assert( message.size() < hSide - 2 );
                    const auto lenMessage = std::min( (int)message.size(), hSide - 2 );
                    for ( int l = 0; l < ( hSide - lenMessage ) / 2 - 1; ++l ) {
                        std::cout << " ";
                    }
                    std::cout << message.substr( 0, lenMessage );
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
