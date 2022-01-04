#pragma once

#include <Core/Utils/Observable.hpp>

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
     *  - OpenGL context profile : Core Profile
     *  The created context is associated with a hidden window that can be shown later.
     * @param size
     */
    explicit OpenGLContext( const std::array<int, 2>& size = { 1, 1 } );
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
    /** Identify the event processing method wheen the window is exposed.
     *
     */
    enum class EventMode : int { POLL = 0, WAIT, TIMEOUT, NUM_MODES };
    /// Show the window
    void show( EventMode mode, float delay );
    /// Hide the window
    void hide();
    /// Resize the window
    void resize( const std::array<int, 2>& size );
    /// loop on events and execute the functor render after each event
    void renderLoop( std::function<void( float )> render );

    /// Give access to the resize event observer collection so that client can add Observer to this
    /// event
    Ra::Core::Utils::Observable<int, int>& resizeListener() { return m_resizers; }

    /** @} */
  private:
    GLFWwindow* m_glfwContext { nullptr };
    /** \addtogroup window
     *  @{
     */
    /// Process the pending events according to the window show mode
    bool processEvents();

    /// Resize callback
    void resizeFrameBuffer( int width, int height );

    Ra::Core::Utils::Observable<int, int> m_resizers;

    /// Event processing mode
    EventMode m_mode { EventMode::POLL };
    /// Timeout delay for event processing
    float m_delay { 1.f / 60.f };
    /** @} */
};
