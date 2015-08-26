#ifndef RADIUMENGINE_BINDABLEMESH_HPP
#define RADIUMENGINE_BINDABLEMESH_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Renderer/Bindable/Bindable.hpp>
#include <Engine/Renderer/RenderTechnique/RenderParameters.hpp>

namespace Ra
{
    namespace Engine
    {
        class Mesh;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API BindableMesh : public Bindable
        {
        public:
            RA_CORE_ALIGNED_NEW
            explicit BindableMesh( Mesh* mesh, uint id );
            virtual ~BindableMesh();

            virtual void bind( ShaderProgram* shader ) const override;
            void render() const;

            bool operator< ( const BindableMesh& other ) const;


            void addRenderParameters( const RenderParameters &parameters )
            {
                m_renderParameters.concatParameters( parameters );
            }

        private:
            Mesh* m_mesh;
            uint m_id;
            Core::Color m_idAsColor;

            RenderParameters m_renderParameters;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLEMESH_HPP
