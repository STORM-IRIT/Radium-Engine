#ifndef RADIUMENGINE_FORWARDRENDERER_HPP
#define RADIUMENGINE_FORWARDRENDERER_HPP

#include <memory>

#include <Engine/Renderer/RenderSystem.hpp>

namespace Ra
{

class Camera;
class ShaderProgram;
class ShaderProgramManager;

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

	ShaderProgram* m_passthroughShader;
	ShaderProgram* m_blinnPhongShader;
	ShaderProgram* m_quadShader;
};

} // namespace Ra

#endif // RADIUMENGINE_FORWARDRENDERER_HPP
