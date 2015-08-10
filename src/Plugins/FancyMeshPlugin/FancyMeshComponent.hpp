#ifndef FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
#define FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP

#include <Engine/Entity/Component.hpp>
#include <Plugins/FancyMeshPlugin/FancyMeshLoadingData.hpp>

namespace Ra
{
    namespace Engine
    {
        struct RenderTechnique;
    }
}

// FIXME(Charly): Do we really want this to be in Engine namespace ?
namespace Ra
{
    namespace Engine
    {

        class RA_API FancyMeshComponent : public Component
        {
        public:
            FancyMeshComponent ( const std::string& name );
            virtual ~FancyMeshComponent();

            virtual void initialize() override;

            void addMeshRenderObject ( const Core::TriangleMesh& mesh, const std::string& name );
            void addMeshRenderObject ( const Core::TriangleMesh& mesh, const std::string& name, RenderTechnique* technique );

            void handleMeshLoading ( const FancyComponentData& data );

        private:
            Core::Index m_renderObject;
        };

    } // namespace Engine
} // namespace Engine

#endif // FANCYMESHPLUGIN_FANCYMESHCOMPONENT_HPP
