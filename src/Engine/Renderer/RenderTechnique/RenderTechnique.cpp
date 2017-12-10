#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Core/Log/Log.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderConfigFactory.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
  namespace Engine
  {

    // For iterating on the enum easily
    const std::array<RenderTechnique::PassName,3> allPasses = {RenderTechnique::Z_PREPASS, RenderTechnique::LIGHTING_OPAQUE, RenderTechnique::LIGHTING_TRANSPARENT};

    std::shared_ptr<Ra::Engine::RenderTechnique> RadiumDefaultRenderTechnique(nullptr);

    RenderTechnique::RenderTechnique()
    {
        for (auto p : allPasses) {
            shaders[p] = nullptr;
        }
    }

    RenderTechnique::RenderTechnique(const RenderTechnique& o)
    {
        material = o.material;
        dirtyBits = o.dirtyBits;
        setPasses = o.setPasses;

        for (auto p : allPasses) {
            if ( setPasses & p )
            {
                shaderConfig[p] = o.shaderConfig.at(p);
                shaders[p] = o.shaders.at(p);
            }
        }
    }

    RenderTechnique::~RenderTechnique()
    {

    }

    void RenderTechnique::setShader( const ShaderConfiguration& newConfig, PassName pass )
    {
        shaderConfig[pass] = newConfig;
        dirtyBits |= pass;
        setPasses  |= pass;
    }

    const ShaderProgram* RenderTechnique::getShader( PassName pass ) const
    {
        if ( setPasses & pass )
        {
            return shaders.at( pass );
        }
        return nullptr;
    }

    void RenderTechnique::updateGL()
    {
        for (auto p : allPasses)
        {
            if ( (setPasses & p) && ( (nullptr == shaders[p]) || ( dirtyBits & p) ) )
            {
                shaders[p] = ShaderProgramManager::getInstance()->getShaderProgram( shaderConfig[p] );
                dirtyBits |= p;
            }
        }

        if ( material )
        {
            material->updateGL();
        }
    }

    const std::shared_ptr<Material>& RenderTechnique::getMaterial() const
    {
        return material;
    }

    void RenderTechnique::resetMaterial( Material* mat )
    {
        material.reset( mat );
    }

    void RenderTechnique::setMaterial( const std::shared_ptr<Material>& material )
    {
        RenderTechnique::material = material;
    }

    ShaderConfiguration RenderTechnique::getConfiguration(PassName pass) const
    {
        return shaders.at(pass)->getBasicConfiguration();
    }

    // creates a Radium default rendertechnique :
    //      Z_PREPASS = Nothing
    //      LIGHTING_OPAQUE = BlinnPhong
    //      LIGHTING_TRANSPARENT = Nothing
    Ra::Engine::RenderTechnique RenderTechnique::createDefaultRenderTechnique()
    {
        if (RadiumDefaultRenderTechnique != nullptr)
            return *(RadiumDefaultRenderTechnique.get());

        Ra::Engine::RenderTechnique *rt = new Ra::Engine::RenderTechnique;

        auto config = ShaderConfigurationFactory::getConfiguration("BlinnPhong");
        rt->setShader(config, LIGHTING_OPAQUE);
        std::shared_ptr<Material> mat(new Material("DefaultGray"));
        rt->setMaterial(mat);
        RadiumDefaultRenderTechnique.reset(rt);
        return *(RadiumDefaultRenderTechnique.get());
    }
  }
} // namespace Ra
