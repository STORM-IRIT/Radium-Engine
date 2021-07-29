#pragma once
#include <array>
#include <functional>
#include <string>
struct GLFWwindow;

/**
 * This class defines GLFW based OpenGL Context.
 * There are two use cases of such a context :
 *   - headless rendering application.
 *   - simple window with limited interaction
 */
class OpenGLContext
{
  public:

    enum class EventMode : int { POLL = 0, WAIT, TIMEOUT, NUM_MODES };

    /** @defgroup context OpenGL context management
     *  These methods allow to create and manipulate an openGLContext.
     *  Using this function, the openGL context created is an offscreen context with no exposed
     *  window.
     *  @{
     */
    /**
     * Create an offscreen openGl context.
     * The created context has the following properties
     *  - OpenGL version requested : >= 4.1
     *  - OpenGL consstext profile : Core Profile
     * @param size
     */
    explicit OpenGLContext( const std::array<int, 2>& size = {1, 1} );
    /// destructor
    ~OpenGLContext();
    /// make the context active
    void makeCurrent() const;
    /// make the context inactive
    void doneCurrent() const;
    /// Check for validity of the context
    bool isValid() const;

    [[nodiscard]] std::string getInfo() const;
    /** @} */

    /** @defgroup window Window management from an openGL context
     *  These methods allow to display and control a simple window associated with the created
     *  OpenGL Context
     *  @{
     */
    /// Show the window
    void show( EventMode mode, float delay );
    /// Hide the window
    void hide();
    /// Resize the window
    void resize( const std::array<int, 2>& size );
    /// Swap back/front buffers
    void swapbuffers();
    /// Wait for the user to close the window
    void waitForClose();
    /// loop on events and execute the functor render after each event
    void renderLoop( std::function<void( float )> render );
    /** @} */

  private:
    GLFWwindow* m_offscreenContext {nullptr};
    EventMode m_mode {EventMode::POLL};
    float m_delay {1.f / 60.f};
};
