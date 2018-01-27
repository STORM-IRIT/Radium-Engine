#ifndef RADIUMENGINE_LIGHT_HPP
#define RADIUMENGINE_LIGHT_HPP

#include <Engine/RaEngine.hpp>
#include <Core/Math/LinearAlgebra.hpp>

namespace Ra
{
    namespace Engine
    {
        class RenderParameters;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API Light
        {
        public:
            enum LightType
            {
                DIRECTIONAL,
                POINT,
                SPOT
            };

        public:
            RA_CORE_ALIGNED_NEW

            Light( const LightType& type );
            virtual ~Light();

            inline const Core::Color& getColor() const;
            inline void setColor( const Core::Color& color );

            virtual void setDirection( const Core::Vector3& dir ) {}
            virtual void setPosition( const Core::Vector3& pos ) {}
            
            inline const LightType& getType() const;

            virtual void getRenderParameters( RenderParameters& params );

        private:
            Core::Color m_color;

            LightType m_type;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/Light.inl>

#endif // RADIUMENGINE_LIGHT_HPP
