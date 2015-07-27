#include <Engine/Renderer/Drawable/Drawable.hpp>

#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>

namespace Ra
{

Engine::Drawable::Drawable(const std::string& name)
	: IndexedObject()
	, m_name(name)
	, m_material(nullptr)
	, m_isDirty(true)
	, m_boundingBox()
{
}

Engine::Drawable::~Drawable()
{
}

void Engine::Drawable::updateGL()
{
	// Do not update while we are cloning
	std::lock_guard<std::mutex> lock(m_updateMutex);
	updateGLInternal();
}

Engine::Drawable* Engine::Drawable::clone()
{
	// Do not clone while we are updating GL internals
	std::lock_guard<std::mutex> lock(m_updateMutex);
	return cloneInternal();
}

Core::Aabb Engine::Drawable::getBoundingBoxInWorld() const
{
	Core::Vector3 minBox = m_boundingBox.min();
	Core::Vector3 maxBox = m_boundingBox.max();
	Core::Vector4 newMin(minBox.x(), minBox.y(), minBox.z(), 1.0);
	Core::Vector4 newMax(maxBox.x(), maxBox.y(), maxBox.z(), 1.0);

	Core::Matrix4 T = m_component->getEntity()->getTransformAsMatrix();
	newMin = T * newMin; newMin /= newMin.w();
	newMax = T * newMax; newMax /= newMax.w();

	minBox = Core::Vector3(newMin.x(), newMin.y(), newMin.z());
	maxBox = Core::Vector3(newMax.x(), newMax.y(), newMax.z());
	Core::Aabb ret(minBox, maxBox);

	return ret;
}

}
