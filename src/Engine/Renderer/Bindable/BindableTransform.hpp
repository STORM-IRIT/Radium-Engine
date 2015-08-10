#ifndef RADIUMENGINE_BINDABLETRANSFORM_HPP
#define RADIUMENGINE_BINDABLETRANSFORM_HPP

#include <Engine/Renderer/Bindable/Bindable.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_API BindableTransform : public Bindable
        {
        public:
            RA_CORE_ALIGNED_NEW
            BindableTransform( const Core::Matrix4& model,
                               const Core::Matrix4& view,
                               const Core::Matrix4& proj );

            ~BindableTransform();

            virtual void bind( ShaderProgram* shader ) const override;

            bool operator< ( const BindableTransform& other ) const;

        private:
            Core::Matrix4 m_model;
            Core::Matrix4 m_view;
            Core::Matrix4 m_proj;
            Core::Matrix4 m_mvp;
            Core::Matrix4 m_modelView;
            Core::Matrix4 m_worldNormal;
            Core::Matrix4 m_viewNormal;
        };

    } // namespace Engine
} // namespace Ra

#endif // RADIUMENGINE_BINDABLETRANSFORM_HPP
