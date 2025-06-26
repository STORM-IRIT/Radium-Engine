#pragma once
#ifdef HEADLESS_HAS_GLFW
#    include <Headless/OpenGLContext/OpenGLContext.hpp>
#    include <Headless/RaHeadless.hpp>
#    include <functional>
#    include <string>

namespace glbinding {
class Version;
} // namespace glbinding

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
    /**
     * Create an offscreen openGl context based on GLFW.
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

    ~GlfwOpenGLContext() override;
    void makeCurrent() const override;
    void doneCurrent() const override;
    bool isValid() const override;
    bool isWindow() const override { return true; }

    [[nodiscard]] std::string getInfo() const override;

    void show( EventMode mode, float delay ) override;
    void hide() override;
    void resize( const std::array<int, 2>& size ) override;
    void renderLoop( std::function<void( float )> render ) override;

  protected:
    bool processEvents() override;

  private:
    GLFWwindow* m_glfwContext { nullptr };
};

} // namespace Headless
} // namespace Ra
#endif
