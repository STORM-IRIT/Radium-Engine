#ifndef RADIUMENGINE_SPOTLIGHT_HPP
#define RADIUMENGINE_SPOTLIGHT_HPP

#include <Engine/RaEngine.hpp>

#include <Core/Math/Math.hpp>
#include <Engine/Renderer/Light/Light.hpp>

namespace Ra
{
    namespace Engine
    {

        class RA_ENGINE_API SpotLight : public Light
        {
        public:
            struct Attenuation
            {
                Scalar constant;
                Scalar linear;
                Scalar quadratic;

                Attenuation() : constant( 1.0 ), linear(), quadratic() {}
            };

        public:
            RA_CORE_ALIGNED_NEW

            SpotLight( const std::string& name = "spotlight" );
            virtual ~SpotLight();

            virtual void getRenderParameters( RenderParameters& params ) override;

            virtual void setPosition( const Core::Vector3& position ) override;
            inline const Core::Vector3& getPosition() const;

            virtual void setDirection( const Core::Vector3& direction ) override;
            inline const Core::Vector3& getDirection() const;

            inline void setInnerAngleInRadians( Scalar angle );
            inline void setOuterAngleInRadians( Scalar angle );
            inline void setInnerAngleInDegrees( Scalar angle );
            inline void setOuterAngleInDegrees( Scalar angle );

            inline Scalar getInnerAngle() const;
            inline Scalar getOuterAngle() const;

            inline void setAttenuation( const Attenuation& attenuation );
            inline void setAttenuation( Scalar constant, Scalar linear, Scalar quadratic );
            inline const Attenuation& getAttenuation() const;

        private:
            Core::Vector3 m_position;
            Core::Vector3 m_direction;

            Scalar m_innerAngle;
            Scalar m_outerAngle;

            Attenuation m_attenuation;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Renderer/Light/SpotLight.inl>

#endif // RADIUMENGINE_SPOTLIGHT_HPP
