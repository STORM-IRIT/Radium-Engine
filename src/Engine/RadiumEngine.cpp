#include <Engine/RadiumEngine.hpp>

#include <thread>
#include <chrono>
#include <mutex>
#include <cstdio>
#include <iostream>

#include <Core/Log/Log.hpp>
#include <Core/String/StringUtils.hpp>
#include <Core/Event/EventEnums.hpp>
#include <Core/Event/KeyEvent.hpp>
#include <Core/Event/MouseEvent.hpp>
#include <Engine/Entity/FrameInfo.hpp>
#include <Engine/Entity/System.hpp>
#include <Engine/Entity/Component.hpp>
#include <Engine/Entity/Entity.hpp>

#include <Engine/Renderer/RenderTechnique/ShaderProgramManager.hpp>
#include <Engine/Renderer/RenderTechnique/ShaderProgram.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>
#include <Engine/Renderer/RenderTechnique/RenderTechnique.hpp>

#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/Mesh/MeshUtils.hpp>

namespace Ra
{
    namespace Engine
    {

        RadiumEngine::RadiumEngine()
            : m_quit ( false )
        {
            //    LOG(INFO) << "Engine starting...";
        }

        RadiumEngine::~RadiumEngine()
        {
        }

        void RadiumEngine::initialize()
        {
            m_renderObjectManager.reset ( new RenderObjectManager );
            m_entityManager.reset ( new EntityManager );
        }

        void RadiumEngine::cleanup()
        {
            for ( auto& system : m_systems )
            {
                system.second.reset();
            }

            m_entityManager.reset();
            m_renderObjectManager.reset();
        }

        void RadiumEngine::getTasks ( Core::TaskQueue* taskQueue,  Scalar dt )
        {
            FrameInfo frameInfo;
            frameInfo.m_dt = dt;
            for ( auto& syst : m_systems )
            {
                syst.second->generateTasks ( taskQueue, frameInfo );
            }
        }

        void RadiumEngine::registerSystem ( const std::string& name, System* system )
        {
            CORE_ASSERT ( m_systems.find ( name ) == m_systems.end(),
                          "Same system added multiple times." );

            m_systems[name] = std::shared_ptr<System> ( system );
        }

        System* RadiumEngine::getSystem ( const std::string& system ) const
        {
            System* sys = nullptr;
            auto it = m_systems.find ( system );

            if ( it != m_systems.end() )
            {
                sys = it->second.get();
            }

            return sys;
        }

        bool RadiumEngine::loadFile ( const std::string& file )
        {
            for ( auto& system : m_systems )
            {
                system.second->handleFileLoading ( file );
            }

            return true;
        }

        bool RadiumEngine::handleKeyEvent ( const Core::KeyEvent& event )
        {
            for ( const auto& system : m_systems )
            {
                if ( system.second->handleKeyEvent ( event ) )
                {
                    return true;
                }
            }

            return false;
        }

        bool RadiumEngine::handleMouseEvent ( const Core::MouseEvent& event )
        {
            for ( const auto& system : m_systems )
            {
                if ( system.second->handleMouseEvent ( event ) )
                {
                    return true;
                }
            }

            return false;
        }

        RenderObjectManager* RadiumEngine::getRenderObjectManager() const
        {
            return m_renderObjectManager.get();
        }

        EntityManager* RadiumEngine::getEntityManager() const
        {
            return m_entityManager.get();
        }
    }
} // namespace RadiumEngine
