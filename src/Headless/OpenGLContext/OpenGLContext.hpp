#pragma once
#include <Core/Utils/Observable.hpp>
#include <Headless/RaHeadless.hpp>

#include <array>
#include <functional>
#include <string>
struct GLFWwindow;

namespace Ra {
namespace Headless {

/**
 * This class defines GLFW based OpenGL Context.
 * There are two use cases of such a context :
 *   - headless rendering application.
 *   - simple window with limited interaction
 */
class HEADLESS_API OpenGLContext
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
     *  These methods allow to display and interact with a simple window associated with the created
     *  OpenGL Context.
     *  @{
     */
    /** Identify the event processing method when the window is exposed.
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

    /// Give access to the resize event observable so that client can add Observer to this
    /// event.
    ///
    /// The parameters sent to the resize listeners are in pixels and correspond to the OpenGL
    /// Framebuffer size (i.e. the size given to glViewport function)
    /// @see https://www.glfw.org/docs/latest/window_guide.html#window_fbsize
    Ra::Core::Utils::Observable<int, int>& resizeListener() { return m_resizers; }

    // TODO : give access to the DPI ratio
    // https://www.glfw.org/docs/latest/window_guide.html#window_scale

    /// Give access to the keyboard event observable so that client can add Observer to this
    /// event.
    ///
    /// The parameters sent to the keyboard listeners are
    /// the keyboard key, platform-specific scancode, key action and modifier bits.
    /// @see https://www.glfw.org/docs/latest/input_guide.html#input_keyboard
    Ra::Core::Utils::Observable<int, int, int, int>& keyboardListener() {
        return m_keyboardObservers;
    }

    /// Give access to the mouse event observable so that client can add Observer to this
    /// event.
    ///
    /// The parameters sent to the mouse listeners are
    /// the mouse button, button action and modifier bits as well as the mouse position
    /// in pixel unit and in the FrameBuffer space. The origin is at the top left of the
    /// framebuffer.
    /// @see https://www.glfw.org/docs/latest/input_guide.html#input_mouse
    Ra::Core::Utils::Observable<int, int, int, int, int>& mouseListener() {
        return m_mouseObservers;
    }

    /// Give access to the mouse move event observable so that client can add Observer to this
    /// event.
    ///
    /// The parameters sent to the mouse move listeners are the mouse position
    /// in pixel unit and in the FrameBuffer space. The origin is at the top left of the
    /// framebuffer.
    /// @see https://www.glfw.org/docs/latest/input_guide.html#cursor_pos
    Ra::Core::Utils::Observable<int, int>& mouseMoveListener() { return m_mouseMoveObservers; }

    /// Give access to the scroll event observable so that client can add Observer to this
    /// event.
    ///
    /// The parameters sent to the scroll listeners are two-dimensional scroll offsets
    /// in pixel unit and in the FrameBuffer space.
    /// @see https://www.glfw.org/docs/latest/input_guide.html#scrolling
    Ra::Core::Utils::Observable<int, int>& scrollListener() { return m_scrollObservers; }

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
    /// Resize event observable
    Ra::Core::Utils::Observable<int, int> m_resizers;

    /// Keyboard callback
    void keyboardEventCallback( int key, int scancode, int action, int mods );
    /// Keyboard event observable
    Ra::Core::Utils::Observable<int, int, int, int> m_keyboardObservers;

    /// Mouse callback
    void mouseEventCallback( int button, int action, int mods, int x, int y );
    /// Mouse event observable
    Ra::Core::Utils::Observable<int, int, int, int, int> m_mouseObservers;

    /// Mouse move callback
    void mouseMoveEventCallback( int x, int y );
    /// Mouse move event observable
    Ra::Core::Utils::Observable<int, int> m_mouseMoveObservers;

    /// Scroll callback
    void scrollEventCallback( int xoffset, int yoffset );
    /// Scroll event observable
    Ra::Core::Utils::Observable<int, int> m_scrollObservers;

    /// Event processing mode
    EventMode m_mode { EventMode::POLL };
    /// Timeout delay for event processing
    float m_delay { 1.f / 60.f };
    /** @} */
};

} // namespace Headless
} // namespace Ra
