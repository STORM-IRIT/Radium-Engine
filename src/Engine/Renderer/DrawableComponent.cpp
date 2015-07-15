#include <Engine/Renderer/DrawableComponent.hpp>

#include <cstdio>
#include <iostream>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Matrix.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Drawable.hpp>
#include <Engine/Renderer/ShaderProgram.hpp>

namespace Ra
{

DrawableComponent::DrawableComponent()
    : Component()
{
}

DrawableComponent::~DrawableComponent()
{
    for (auto& drawable : m_drawables)
    {
        // Keep sure only this DrawableComponent has ownership on its drawables
        CORE_ASSERT(drawable.second.unique(), "Non-unique drawable about to be removed.");
        drawable.second.reset();
    }

    m_drawables.clear();
}

void DrawableComponent::update()
{
    m_shaderProgram->setUniform("model", m_entity->getTransformAsMatrix());

    for (const auto& drawable : m_drawables)
    {
        drawable.second->draw();
    }
}

void DrawableComponent::addDrawable(Drawable* drawable)
{
    std::string name = drawable->getName();

    if (m_drawables.find(name) != m_drawables.end())
    {
        fprintf(stderr, "Warning, Drawable %s has already been added.\n", name.c_str());
        return;
    }

    m_drawables.insert(DrawableByName(name, std::shared_ptr<Drawable>(drawable)));
}

void DrawableComponent::removeDrawable(const std::string& name)
{
    auto drawable = m_drawables.find(name);
    if (drawable == m_drawables.end())
    {
        fprintf(stderr, "Warning, Drawable %s does not exist.\n", name.c_str());
        return;
    }

    CORE_ASSERT(drawable->second.unique(), "Non-unique drawable about to be removed.");
    drawable->second.reset();
    m_drawables.erase(drawable);
}

void DrawableComponent::removeDrawable(Drawable* drawable)
{
    removeDrawable(drawable->getName());
}

Drawable* DrawableComponent::getDrawable(const std::string &name) const
{
    auto drawable = m_drawables.find(name);
    Drawable* ret = nullptr;

    if (drawable != m_drawables.end())
    {
        ret = drawable->second.get();
    }

    return ret;
}

std::vector<Drawable*> DrawableComponent::getDrawables() const
{
    std::vector<Drawable*> drawables;
    drawables.reserve(m_drawables.size());

    for (const auto& drawable : m_drawables)
    {
        drawables.push_back(drawable.second.get());
    }

    return drawables;
}

void DrawableComponent::setShaderProgram(ShaderProgram* shader)
{
    m_shaderProgram = shader;
}

} // namespace Ra
