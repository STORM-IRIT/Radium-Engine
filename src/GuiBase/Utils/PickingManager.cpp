#include <GuiBase/Utils/PickingManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Core/Containers/MakeShared.hpp>

#include <Engine/Renderer/RenderObject/RenderObject.hpp>
#include <Engine/Managers/EntityManager/EntityManager.hpp>
#include <Engine/Renderer/Mesh/Mesh.hpp>
#include <Engine/Renderer/RenderTechnique/Material.hpp>

namespace Ra
{
    namespace Gui
    {

        PickingManager::PickingManager()
        {
            m_pickingResult.m_mode  = Engine::Renderer::RO;
            m_pickingResult.m_roIdx = -1;
        }

        PickingManager::~PickingManager()
        {}

        void PickingManager::setCurrent( const Engine::Renderer::PickingResult& pr )
        {
            m_pickingResult = pr;
        }

        const Engine::Renderer::PickingResult& PickingManager::getCurrent() const
        {
            return m_pickingResult;
        }

        void PickingManager::clear()
        {
            m_pickingResult.m_mode = Engine::Renderer::RO;
            m_pickingResult.m_roIdx = Core::InvalidIdx;
            m_pickingResult.m_vertexIdx.clear();
            m_pickingResult.m_edgeIdx.clear();
            m_pickingResult.m_elementIdx.clear();
        }

    }
}

