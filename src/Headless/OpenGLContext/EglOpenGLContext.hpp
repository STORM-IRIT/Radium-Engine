#pragma once
#ifdef HEADLESS_HAS_EGL
#    include <Headless/OpenGLContext/OpenGLContext.hpp>
#    include <Headless/RaHeadless.hpp>
#    include <memory>
#    include <string>

namespace glbinding {
class Version;
} // namespace glbinding

namespace Ra {
namespace Headless {

/**
 * This class defines EGL based OpenGL Context.
 * There are one use case of such a context :
 *   - headless rendering application.
 *   (no simple window management right now)
 */
class HEADLESS_API EglOpenGLContext : public OpenGLContext
{
  public:
    explicit EglOpenGLContext( const glbinding::Version& glVersion = { 4, 1 },
                               const std::array<int, 2>& size      = { { 1, 1 } } );
    ~EglOpenGLContext() override;

    void makeCurrent() const override;
    void doneCurrent() const override;
    bool isValid() const override;

    [[nodiscard]] std::string getInfo() const override;

  private:
    struct ContextEGL;
    std::unique_ptr<ContextEGL> m_eglContext;
};

} // namespace Headless
} // namespace Ra
#endif
