#ifndef RADIUMENGINE_RENDERER_HPP
#define RADIUMENGINE_RENDERER_HPP

#include <string>

#include <Core/CoreMacros.hpp>
#include <Engine/Entity/System.hpp>

namespace Ra { namespace Engine {

class RenderSystem : public System
{
public:
	/// CONSTRUCTOR
    RenderSystem() : System() {}

	/// DESTRUCTOR
    virtual ~RenderSystem() = default;

	/** 
     * @brief Initialize generic renderer stuff. Nothing by default.
	 */
    virtual void initialize() override{}

    /**
     * @brief Initialize OpenGL stuff.
     */
    virtual void initializeGL(uint width, uint height) = 0;

    /**
     * @brief Render the scene
     */
    virtual void render() = 0;

	/**
     * @brief Update stuff in the scene, does nothing by default.
	 */
    virtual void update() override {}

    virtual void update(Scalar dt) override {}

	/** 
	 * @brief Resize the viewport. Must be overrided.
	 * @param width New viewport width
	 * @param height New viewport height
	 */
	virtual void resize(uint width, uint height) = 0;

    // TODO (Charly):	Define a MouseEvent and KeyboardEvent structure,
	//					and send events from the GUI to the renderer.
	/** 
	 * @brief Handle a keyboard event from the GUI.
	 * @param event The event description
	 * @return Boolean telling caller if the event has been consumed (true) or not (false).
	 * If the event has not been consumed, the basic behaviour will be to broadcast the event
	 * to the other GUI components.
	 */
	//virtual bool handleKeyEvent(const KeyEvent& event) = 0;

	/**
	* @brief Handle a mouse event from the GUI.
	* @param event The event description
	* @return Boolean telling caller if the event has been consumed (true) or not (false).
	* If the event has not been consumed, the basic behaviour will be to broadcast the event
	* to the other GUI components.
	*/
	//virtual bool handleMouseEvent(const MouseEvent& event) = 0;

	/** 
	 * @brief Getter for the renderer viewport width.
	 * @note This method is marked final since there is no point to override it.
	 * @return Current viewport width.
	 */
	virtual uint getWidth() const final { return m_width; }

	/**
	* @brief Getter for the renderer viewport height.
	* @note This method is marked final since there is no point to override it.
	* @return Current viewport height.
	*/
	virtual uint getHeight() const final { return m_height; }

	/** 
	 * @brief Take a screenshot of the current renderer state
	 * This method does not have to be overrided, but does nothing by default.
	 * @param filename The file to save the screenshot to
	 * @return True if the screenshot has been saved successfully, false otherwise.
	 */
	virtual bool saveScreenshot(const std::string& filename) const { return true; }

	/** 
	 * @brief Change displayed texture (mostly for debug purposes).
	 * This method does not have to be overrided, but does nothing by default.
	 * @param texIdx The index of the texture to display.
	 */
	virtual void debugTexture(uint texIdx) {}

protected:
	uint m_width;
	uint m_height;
};

} // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_RENDERER_HPP
