#ifndef RADIUMENGINE_DIRLIGHT_HPP
#define RADIUMENGINE_DIRLIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API DirectionalLight : public Light
        {
        public:
            RA_CORE_ALIGNED_NEW

            DirectionalLight( const std::string& name = "dirlight" );
            virtual ~DirectionalLight();

            virtual void getRenderParameters( RenderParameters& params ) override;

            virtual void setDirection( const Core::Vector3& pos ) override;
            inline const Core::Vector3& getDirection() const;

        private:
            Core::Vector3 m_direction;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/DirLight.inl>

#endif // RADIUMENGINE_POINTLIGHT_HPP
