#include <RadiumHeadless/CLIViewer.hpp>
#include <iostream>

#include <Engine/Rendering/ForwardRenderer.hpp>

#include <Core/Resources/Resources.hpp>

#ifdef ADD_ASSIMP_LOADER
#    include <IO/AssimpLoader/AssimpFileLoader.hpp>
#endif

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

int main( int argc, const char* argv[] ) {
    //! [Creating the viewer with custom parameters]
    bool showWindow {false};
    CLIViewer viewer;
    viewer.add_flag( "-w,--window", showWindow, "Map the viewer window." );
    //! [Creating the viewer with custom parameters]

    //! [Configuring the viewer : initialize OpenGL and the Engine]
    if ( int code = viewer.init( argc, argv ) ) { return code; }
    auto viewerParameters = viewer.getCommandLineParameters();
    //! [Configuring the viewer : initialize OpenGL and the Engine]

    //! [Populating the viewer with needed services]
    viewer.setRenderer( new Ra::Engine::Rendering::ForwardRenderer() );
#ifdef ADD_ASSIMP_LOADER
    viewer.addDataFileLoader( new Ra::IO::AssimpFileLoader() );
#endif
    //! [Populating the viewer with needed services]

    //! [Bind the OpenGLContext if needed]
    viewer.bindOpenGLContext();
    //! [Bind the OpenGLContext if needed]

    //! [Setup the Scene to render]
    if ( viewer.getDataFileName().empty() )
    {
        auto rp              = Ra::Core::Resources::getResourcesPath();
        std::string filename = *rp + "/Demos/Assets/radium-logo.dae";
        viewer.setDataFileName( filename );
    }
    viewer.loadScene();
    viewer.compileScene();
    viewer.setCamera();
    //! [Setup the Scene to render]

    //! [Running the application]
    auto saveFrame = [&viewer]( const std::string& filename ) {
        size_t w, h;
        std::cout << "Writing image " << filename << std::endl;
        auto writtenPixels = viewer.grabFrame( w, h );
        stbi_write_png( filename.c_str(), w, h, 4, writtenPixels.get(), w * 4 * sizeof( uchar ) );
    };

    auto render = [&viewer]( float time_step ) { viewer.oneFrame( time_step ); };

    if ( showWindow )
    {
        if ( viewerParameters.m_animationEnable )
        { viewer.showWindow( true, OpenGLContext::EventMode::TIMEOUT ); }
        else
        { viewer.showWindow( true, OpenGLContext::EventMode::WAIT ); }
        viewer.renderLoop( render );
        viewer.showWindow( false );
    }
    else
    {
        if ( viewerParameters.m_animationEnable )
        {
            // compute 2s of animation at 30fps
            float duration = 2;
            float fps      = 30;
            for ( int i = 0; i < duration * fps; ++i )
            {
                render( 1.f / fps );
                auto filename = viewerParameters.m_imgPrefix + std::to_string( i ) + ".png";
                saveFrame( filename );
            }
        }
        else
        {
            // compute one picture
            render( 0 );
            auto filename = viewerParameters.m_imgPrefix + ".png";
            saveFrame( filename );
        }
    }
    //! [Running the application]

    //! [Unbind the OpenGLContext when no more needed]
    viewer.bindOpenGLContext( false );
    //! [Unbind the OpenGLContext when no more needed]
    return 0;
}
