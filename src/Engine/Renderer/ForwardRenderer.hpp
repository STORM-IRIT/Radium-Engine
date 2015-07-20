#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <vector>
#include <array>
#include <memory>

#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra { namespace Engine { class Camera;               } }
namespace Ra { namespace Engine { class ShaderProgram;        } }
namespace Ra { namespace Engine { class ShaderProgramManager; } }
namespace Ra { namespace Engine { class Light;                } }
namespace Ra { namespace Engine { class FBO;                  } }
namespace Ra { namespace Engine { class Texture;              } }
namespace Ra { namespace Engine { class Mesh;                 } }

namespace Ra { namespace Engine {

class ForwardRenderer : public RenderSystem
{
public:
    enum TexturesFBO
    {
        TEXTURE_DEPTH = 0,
        TEXTURE_COLOR = 1,
        TEXTURE_COUNT
    };

public:
	/// CONSTRUCTOR 
	ForwardRenderer();

	/// DESCTRUCTOR
	virtual ~ForwardRenderer();

    virtual void initializeGL(uint width, uint height) override;

    virtual void render() override;

	virtual void resize(uint width, uint height) override;

    virtual bool handleMouseEvent(const Core::MouseEvent& event) override;
    virtual bool handleKeyEvent(const Core::KeyEvent& event) override;

    virtual void debugTexture(uint texIdx) override;

private:
	void initShaders();
	void initBuffers();

public:
	std::shared_ptr<Camera> m_camera;
	
	ShaderProgramManager* m_shaderManager;

    ShaderProgram* m_depthShader;
    ShaderProgram* m_quadShader;
    Mesh* m_quadMesh;

    FBO* m_fbo;

    std::array<Texture*, TEXTURE_COUNT> m_textures;
    Texture* m_displayedTexture;
    bool     m_displayedIsDepth;

    Scalar m_lastMouseX;
    Scalar m_lastMouseY;
    bool m_camRotateStarted;
    bool m_camZoomStarted;
    bool m_camPanStarted;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
