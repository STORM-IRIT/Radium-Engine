#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <vector>
#include <memory>

#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra { namespace Engine { class Camera;               } }
namespace Ra { namespace Engine { class ShaderProgram;        } }
namespace Ra { namespace Engine { class ShaderProgramManager; } }
namespace Ra { namespace Engine { class Light;                } }

namespace Ra { namespace Engine {

class ForwardRenderer : public RenderSystem
{
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

private:
	void initShaders();
	void initBuffers();

private:
	std::shared_ptr<Camera> m_camera;
	
	ShaderProgramManager* m_shaderManager;

    std::vector<Light*> m_lights;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
