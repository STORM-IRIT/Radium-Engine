#pragma once
#include <RadiumHeadless/CLIBaseApplication.hpp>
#include <RadiumHeadless/OpenGLContext/OpenGLContext.hpp>

#include <Core/Utils/Index.hpp>

#include <functional>

namespace Ra {
namespace Core {
namespace Asset {
class FileLoaderInterface;
class Camera;
} // namespace Asset
} // namespace Core

namespace Engine {
class RadiumEngine;
namespace Rendering {
class Renderer;
}
} // namespace Engine

namespace Headless {
/**
 * Base class for radium based cmdline application
 */
class HEADLESS_API CLIViewer : public CLIBaseApplication
{
  public:
    struct ViewerParameters {
        /// Load animation system at startup
        bool m_animationEnable { false };
        /// Size of the image
        std::array<int, 2> m_size { 512, 512 };
        /// image name prefix
        std::string m_imgPrefix { "frame" };
        /// The data file to manage
        std::string m_dataFile = { "" };
    };

  private:
    /// Headless OpenGLContext
    OpenGLContext m_glContext;
    /// Instance of the radium engine.
    std::unique_ptr<Ra::Engine::RadiumEngine> m_engine;
    /// Shared instance of the renderer
    std::shared_ptr<Ra::Engine::Rendering::Renderer> m_renderer;

    /// The camera for rendering
    Ra::Core::Asset::Camera* m_camera { nullptr };

    /// The application parameters
    ViewerParameters m_parameters;

    /// is the engine initialized ?
    bool m_engineInitialized { false };

    /// is the window shown ?
    bool m_exposedWindow { false };

  public:
    /// Base constructor.
    CLIViewer();
    /// Base destructor
    virtual ~CLIViewer();

    /**
     * \brief Application initialization method.
     *
     * This method is called by the main function to initialize the app giving its parameters.
     * @param argc number of parameter
     * @param argv array of string representing the parameters
     * @return 0 if the application is correctly initialized or an application dependant error code
     * if something went wrong.
     *
     * Supported command line parameters are --help and --file <filename> from CLIRadiumApplication
     * and the following parameters :
     *   - --size <width x height> : the size of the rendered picture
     *   - --animation : load the Radium animation system
     *   - --env <env_map> : load and use the given environment map.
     */
    int init( int argc, const char* argv[] ) override;

    /**
     * Render one frame of the scene attached to the engine for the given time
     * stamp.
     *
     * @return 0 if the image was correctly computed or an application dependant error code if
     * something went wrong.
     */
    int oneFrame( float timeStep = 1.f / 60.f );

    /**
     * Set the renderer to use to compute an image
     *  The app takes ownership of the give pointer.
     */
    void setRenderer( Ra::Engine::Rendering::Renderer* renderer );

    /**
     * Add a data loader to the engine
     */
    void addDataFileLoader( Ra::Core::Asset::FileLoaderInterface* loader );

    /**
     * Loads the scene from the datafile given at the command line or set using setDataFileName
     * using the available loader (if any).
     */
    void loadScene();

    /** Instantiate the scene and prepare the rendering.
     * This method prepare all render techniques for the active renderer.
     */
    void compileScene();

    /** Register OpenGL addons
     *  The given functor will be called with the OpenGL context bound
     */
    void openGlAddOns( std::function<void()> f );

    /** Activate/deactivate the OpenGL context */
    void bindOpenGLContext( bool on = true );

    /** Define the cq;erq to be used for rendering.
     * This method set the camera either using default camera or using the first one (at index 0)
     * from the engine's camera manager and  */
    void setCamera( Ra::Core::Utils::Index camIdx = 0 );

    /** Set the image prefix ("frame" by default) */
    void setImageNamePrefix( std::string s );

    /** Get the current viewer command line parameters parameters */
    const ViewerParameters& getCommandLineParameters() const;

    /** grab the last rendered frame */
    std::unique_ptr<unsigned char[]> grabFrame( size_t& w, size_t& h ) const;

    /** show/hide the opengl window */
    void showWindow( bool on                       = true,
                     OpenGLContext::EventMode mode = OpenGLContext::EventMode::POLL,
                     float delay                   = 1.f / 60.f );

    /** If a window is shown, launch the interactive render loop */
    void renderLoop( std::function<void( float )> render );

    /**
     * Get the filename given using the option --file <filename> or -f <filename>
     */
    inline std::string getDataFileName() const;

    /**
     * Set the filename given using the option --file <filename> or -f <filename>
     */
    inline void setDataFileName( std::string filename );

  private:
    void resize( int width, int height );
};

} // namespace Headless
} // namespace Ra

#include <RadiumHeadless/CLIViewer.inl>
