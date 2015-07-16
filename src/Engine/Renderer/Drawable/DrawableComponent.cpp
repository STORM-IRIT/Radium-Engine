#include <Engine/Renderer/Drawable/DrawableComponent.hpp>

#include <cstdio>
#include <iostream>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Matrix.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>

namespace Ra
{

Engine::DrawableComponent::DrawableComponent()
    : Engine::Component()
{
}

Engine::DrawableComponent::~DrawableComponent()
{
    for (auto& drawable : m_drawables)
    {
        // Keep sure only this DrawableComponent has ownership on its drawables
        CORE_ASSERT(drawable.second.unique(), "Non-unique drawable about to be removed.");
        drawable.second.reset();
    }

    m_drawables.clear();
}

void Engine::DrawableComponent::update()
{
    m_shaderProgram->setUniform("model", m_entity->getTransformAsMatrix());

    for (const auto& drawable : m_drawables)
    {
        drawable.second->draw();
    }
}

void Engine::DrawableComponent::addDrawable(Engine::Drawable* drawable)
{
    std::string name = drawable->getName();

    if (m_drawables.find(name) != m_drawables.end())
    {
        fprintf(stderr, "Warning, Drawable %s has already been added.\n", name.c_str());
        return;
    }

    m_drawables.insert(DrawableByName(name, std::shared_ptr<Drawable>(drawable)));
}

void Engine::DrawableComponent::removeDrawable(const std::string& name)
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

void Engine::DrawableComponent::removeDrawable(Engine::Drawable* drawable)
{
    removeDrawable(drawable->getName());
}

Engine::Drawable* Engine::DrawableComponent::getDrawable(const std::string &name) const
{
    auto drawable = m_drawables.find(name);
    Engine::Drawable* ret = nullptr;

    if (drawable != m_drawables.end())
    {
        ret = drawable->second.get();
    }

    return ret;
}

std::vector<Engine::Drawable*> Engine::DrawableComponent::getDrawables() const
{
    std::vector<Engine::Drawable*> drawables;
    drawables.reserve(m_drawables.size());

    for (const auto& drawable : m_drawables)
    {
        drawables.push_back(drawable.second.get());
    }

    return drawables;
}

void Engine::DrawableComponent::setShaderProgram(Engine::ShaderProgram* shader)
{
    m_shaderProgram = shader;
}

} // namespace Ra
