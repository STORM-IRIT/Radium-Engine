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
        class RenderObject;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API BindableMesh
        {
        public:
            RA_CORE_ALIGNED_NEW
            explicit BindableMesh( RenderObject* mesh, uint id );
            virtual ~BindableMesh();

            void bind() const;
            void bind( const RenderParameters& params ) const;
            void bind( ShaderProgram* shader ) const;
            void bind( ShaderProgram* shader, const RenderParameters& params ) const;

            void render() const;

            bool operator< ( const BindableMesh& other ) const;

        private:
            RenderObject* m_mesh;
            uint m_id;
            Core::Color m_idAsColor;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLEMESH_HPP
