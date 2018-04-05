#ifndef RADIUMENGINE_DUMMYLIGHTMANAGER_HPP
#define RADIUMENGINE_DUMMYLIGHTMANAGER_HPP

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/Light/DirLight.hpp>
#include <Engine/Managers/LightManager/LightManager.hpp>

#include <memory>
#include <vector>


namespace Ra {
    namespace Engine {

        /**
         * Associated class.
         */
        class RA_ENGINE_API DummyLightStorage : public LightStorage
        {
        public:
            DummyLightStorage();
            void push(Light* i) override;
            void upload() const override;
            size_t size() const override;
            void clear() override;
            Light* operator[](unsigned int n) override;

        private:
            // TODO (Hugo) Should handle other simple types of light.
            std::vector<std::shared_ptr<DirectionalLight>> m_lights;
        };



        /**
         * @brief DummyLightManager. A simple Light Manager with a list of lights.
         */
        class RA_ENGINE_API DummyLightManager : public LightManager
        {
        public:
            DummyLightManager();

            const Light* getLight(uint li) const;

            // Since this manager is dummy, it won't do anything here.
            void preprocess(const RenderData &) override;
            void prerender(unsigned int li) override;
            void render(RenderObject*, unsigned int li, RenderTechnique::PassName passname = RenderTechnique::LIGHTING_OPAQUE);
            void postrender(unsigned int li) override;
            void postprocess() override;
        };

    }
}

#endif // DUMMYLIGHTMANAGER_HPP
