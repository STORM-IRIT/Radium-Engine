#include "DummyLightManager.hpp"

#include <Engine/RadiumEngine.hpp>
#include <Engine/Renderer/OpenGL/OpenGL.hpp>

namespace Ra {
    namespace Engine {

        DummyLightManager::DummyLightManager()
        {
            m_data.reset( new DummyLightStorage() );

            // Add a dummy light FIXME (Hugo).
            DirectionalLight l;
            l.setDirection(Core::Vector3(0.3f, -1.0f, 0.0f));
            m_data->push(l);
        }

        //
        // Pre/Post render operations.
        //

        void DummyLightManager::preprocess()
        {
            GL_ASSERT(glDepthFunc(GL_LEQUAL));
            GL_ASSERT(glDepthMask(GL_FALSE));

            GL_ASSERT(glEnable(GL_BLEND));
            GL_ASSERT(glBlendFunc(GL_ONE, GL_ONE));
        }

        void DummyLightManager::prerender(unsigned int li, RenderParameters& params)
        {
            Light& light = (*m_data.get())[li];
            light.getRenderParameters(params);
        }

        void DummyLightManager::postrender(unsigned int li)
        {
        }

        void DummyLightManager::postprocess()
        {
            // Eventually, this would be a good idea to disable GL_BLEND, and
            // all what was enabled in preprocess().
        }



        DummyLightStorage::DummyLightStorage()
        {
        }

        void DummyLightStorage::upload() const
        {
        }

        void DummyLightStorage::push(Light& li)
        {
            // FIXME (Hugo) This code shouldn't exist.
            // It was done for deadline purpose :)
            // It will be removed.
            m_lights.push_back( *static_cast<DirectionalLight*>(&li) );
        }

        size_t DummyLightStorage::size() const
        {
            return m_lights.size();
        }

        void DummyLightStorage::clear()
        {
            m_lights.clear();
        }

        Light& DummyLightStorage::operator[](unsigned int n)
        {
            return m_lights[n];
        }
    }
}
