#ifndef RADIUMENGINE_RENDERTECHNIQUE_HPP
#define RADIUMENGINE_RENDERTECHNIQUE_HPP

#include <Engine/RaEngine.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderConfiguration.hpp>


#include <memory>
#include <map>

namespace Ra
{
    namespace Engine
    {
        class ShaderProgram;
        class Material;
    }
}

namespace Ra
{
    namespace Engine
    {
    // TODO (Mathias) : Adapt RenderTechnique for multi-material purpose
    // TODO transform the folowing ideas and insight into a real doc ...
    //      --> Interface that must be implemented by a RenderTechnique
    //      --> depthAmbiant Pass
    //              This pass must compute the Z of each Fragment and, optionally, the ambiant term
    //              (see DepthAmbiantPass shader of Forward Renderer)
    //      --> opaqueLighting Pass
    //              This pass must compute the lighting of the opaque aspect of the material
    //              (see BlinnPhong shader of Forward Renderer)
    //      --> transparentLighting Pass
    //              This pass must compute the lighting of the transparent aspect of the material
    //              (see LitOIT shader of Forward Renderer)
    //
    //  --> modify RenderObject so that it is able to activate the required technique for rendering
    //      This is done but not yet finalized ... main rendering (internal render) is fixed, secondary renderings (ui,
    //          debug, ...) still have the possibilities to bypass the notion of Technique"
    //
    // A RenderTechnique correspond to a set shader configuration allowing to manage a Material while rendering.
    //   The set of configurations must contains what is required to render objects in the following ways :
    //      1- depth and ambiant-environment lighting :
    //          Required for the depth pre-pass of several renderers.
    //          Must initialise the color. Initialization at 0 or to the base color of the fragments resulting from
    //          ambiant-Environment lighting
    //          * Default/Reference : DepthAmbiantPass shaders
    //      2- Opaque lighting : THIS ONE IS MANDATORY, EACH MATERIAL MUST AT LEAST BE RENDERABLE AS OPAQUE OBJECT
    //          Main configuration, computes the resluting color according to a lighting configuration.
    //          The lighting configuration might contains one or severa sources of different types.
    //          * Default/Reference : BlinnPhong  shaders
    //      3- Transparent lighting :
    //          Same as opaque lighting but for transparent objects
    //          * Default/Reference LitOIT shaders
    //      4- WhatElse ????
    //
/*  Exemple of use from Forward Renderer
 *
 *  depthAmbiant pass
 *      Build a RenderParameters object defining the ambiant lighting configuration (envmap or irradiancemap or
 *          constant factor of base color.
 *      for each RenderObject
 *          call ro->render(ambiantLigthingPtarams, renderData(stange name for matrices + time), TECHNIQUE_DEPTH_AMBIANT);
 *
 *  opaqueLighting pass :
 *      For each light sources set (from 1 to N light sources)
 *          Build a RenderParameters object defining the lighting configuration.
 *          For each RenderObject
 *              call ro->render(ambiantLigthingPtarams, renderData(stange name for matrices + time), TECHNIQUE_LIGHT_OPAQUE);
 *
 *  transparentLighting pass
 *      For each light sources set (from 1 to N light sources)
 *          Build a RenderParameters object defining the lighting configuration.
 *          For each RenderObject
 *              call ro->render(ambiantLigthingPtarams, renderData(stange name for matrices + time), TECHNIQUE_LIGHT_TRANSPARENT);
 *
 *  Each technique must be configured with its own shaders.
 *
 *   TODO : Default rendertechnique must be renderer dependant ...
 */
        class RenderTechnique
        {
        public:
            enum PassName {
                Z_PREPASS               = 1 << 0,
                LIGHTING_OPAQUE         = 1 << 1,
                LIGHTING_TRANSPARENT    = 1 << 2,
                NO_PASS                 = 0
            };

            RenderTechnique();
            RenderTechnique(const RenderTechnique&);
            ~RenderTechnique();

            RA_ENGINE_API void setShader( const ShaderConfiguration& newConfig, PassName pass = LIGHTING_OPAQUE );
            RA_ENGINE_API const ShaderProgram* getShader( PassName pass = LIGHTING_OPAQUE ) const;
            RA_ENGINE_API ShaderConfiguration getConfiguration( PassName pass = LIGHTING_OPAQUE ) const;

            RA_ENGINE_API const std::shared_ptr<Material>& getMaterial() const;
            RA_ENGINE_API void resetMaterial( Material* mat );
            RA_ENGINE_API void setMaterial( const std::shared_ptr<Material>& material );
            RA_ENGINE_API void updateGL();
            RA_ENGINE_API bool shaderIsDirty ( PassName pass = LIGHTING_OPAQUE ) const;

            // creates a Radium default rendertechnique :
            //      Z_PREPASS = DepthDepthAmbientPass
            //      LIGHTING_OPAQUE = BlinnPhong
            //      LIGHTING_TRANSPARENT = LitOIT
            static RA_ENGINE_API Ra::Engine::RenderTechnique createDefaultRenderTechnique();
        private:
            using ConfigurationSet = std::map<PassName, ShaderConfiguration>;
            using ShaderSet = std::map<PassName, const ShaderProgram*>;
            ConfigurationSet shaderConfig;
            ShaderSet shaders;

            std::shared_ptr<Material> material = nullptr;

            // Change this if there is more than 8 configurations
            unsigned char dirtyBits = (Z_PREPASS | LIGHTING_OPAQUE | LIGHTING_TRANSPARENT);
            unsigned char setPasses = NO_PASS;



        };

    } // namespace Engine
} // namespace Ra


#endif // RADIUMENGINE_RENDERTECHNIQUE_HPP
