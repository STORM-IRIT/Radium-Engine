#include <Engine/Renderer/Drawable/DrawableComponent.hpp>

#include <cstdio>
#include <iostream>

#include <Core/CoreMacros.hpp>
#include <Core/Math/Matrix.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Renderer/Material/Material.hpp>
#include <Engine/Renderer/Drawable/Drawable.hpp>
#include <Engine/Renderer/Shader/ShaderProgram.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra
{

Engine::DrawableComponent::DrawableComponent()
    : Engine::Component()
    , m_material(nullptr)
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

    delete m_material;
}

void Engine::DrawableComponent::draw(const Core::Matrix4& viewMatrix,
                                     const Core::Matrix4& projMatrix,
                                     Light* light)
{
    // TODO(Charly): Potential optimization
    //                  -> Keep sure all materials are ordered.
    if (nullptr == m_material)
    {
        return;
    }

    ShaderProgram* shader = m_material->getCurrentShaderProgram();
    if (nullptr == shader)
    {
        return;
    }

    shader->bind();

    Core::Matrix4 modelMatrix = m_entity->getTransformAsMatrix();
    Core::Matrix4 mvpMatrix = projMatrix * viewMatrix * modelMatrix;
    // TODO(Charly): Add normal matrix

    shader->setUniform("model", modelMatrix);
    shader->setUniform("view", viewMatrix);
    shader->setUniform("proj", projMatrix);
    shader->setUniform("mvp", mvpMatrix);

    m_material->bind();
    light->bind(shader);

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

void Engine::DrawableComponent::setMaterial(Material* material)
{
    m_material = material;
}

void Engine::DrawableComponent::setSelected(bool selected)
{
    m_isSelected = selected;

    if (selected)
    {
        m_material->changeMode(Material::MODE_CONTOUR);
    }
    else
    {
        m_material->changeMode(Material::MODE_DEFAULT);
    }
}

} // namespace Ra
