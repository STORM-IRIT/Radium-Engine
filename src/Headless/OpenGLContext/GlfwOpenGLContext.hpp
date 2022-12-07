#pragma once
#include <Headless/OpenGLContext/OpenGLContext.hpp>

struct GLFWwindow;

namespace Ra {
namespace Headless {

/**
 * This class defines GLFW based OpenGL Context.
 * There are two use cases of such a context :
 *   - headless rendering application.
 *   - simple window with limited interaction
 */
class HEADLESS_API GlfwOpenGLContext : public OpenGLContext
{
  public:
    /** @defgroup context OpenGL context management
     *  These methods allow to create and manipulate an openGLContext.
     *  Using this function, the openGL context created is an offscreen context with no exposed
     *  window.
     *  @{
     */
    /**
     * Create an offscreen openGl context.
     * The created context has the following properties
     *  - OpenGL version requested : (default >= 4.1)
     *  - OpenGL context profile : Core Profile
     *  The created context is associated with a hidden window that can be shown later.
     *  \param glVersion (optional, default is 4.1) indicates the OpenGL version the context MUST
     *  be compatible with. If there is no compatible context, the application will stop
     * \param size
     */
    explicit GlfwOpenGLContext( const glbinding::Version& glVersion = { 4, 1 },
                                const std::array<int, 2>& size      = { { 1, 1 } } );
    /// destructor
    ~GlfwOpenGLContext() override;
    /// make the context active
    void makeCurrent() const override;
    /// make the context inactive
    void doneCurrent() const override;
    /// Check for validity of the context
    bool isValid() const override;

    [[nodiscard]] std::string getInfo() const override;
    /** @} */

    /** @defgroup window Window management from an openGL context
     *  These methods allow to display and interact with a simple window associated with the created
     *  OpenGL Context.
     *  @{
     */
    /** Identify the event processing method when the window is exposed.
     *
     */
    /// Show the window
    void show( EventMode mode, float delay ) override;
    /// Hide the window
    void hide() override;
    /// Resize the window
    void resize( const std::array<int, 2>& size ) override;
    /// loop on events and execute the functor render after each event
    void renderLoop( std::function<void( float )> render ) override;

    /** @} */
  protected:
    /// Process the pending events according to the window show mode
    bool processEvents() override;

  private:
    GLFWwindow* m_glfwContext { nullptr };
};

} // namespace Headless
} // namespace Ra
