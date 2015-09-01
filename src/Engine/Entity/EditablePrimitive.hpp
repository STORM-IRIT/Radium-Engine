#ifndef RADIUMENGINE_EDITABLE_PRIMITIVE_HPP_
#define RADIUMENGINE_EDITABLE_PRIMITIVE_HPP_

#include <Engine/RaEngine.hpp>

#include <string>
#include <vector>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>


namespace Ra
{
    namespace Engine
    {
        /// A class representing a property that can be edited through the GUI.
        /// using a "Variant" representation.
        class EditablePrimitive
        {
        public:
            enum Type
            {
                SCALAR,
                POSITION,
                ROTATION,
                SCALE,
                COLOR,
            };

            // TODO: use Core::Any ?
            union Variant
            {
                // We need to define the special functions.
                Variant() {}

                Scalar scalar;
                Core::Vector3 position;
                Core::Quaternion rotation;
                Core::Vector3 scale;
                Core::Color color;
            };

            /// Factory methods.
            static inline EditablePrimitive scalar( const std::string& name, Scalar scalar = 0.f );
            static inline EditablePrimitive position( const std::string& name,
                                                     const Core::Vector3& vector  = Core::Vector3::Zero() );
            static inline EditablePrimitive rotation( const std::string& name,
                                                     const Core::Quaternion& quat = Core::Quaternion::Identity() );
            static inline EditablePrimitive scale( const std::string& name, const Core::Vector3& scale   = Core::Vector3::Ones() );
            static inline EditablePrimitive color( const std::string& name, const Core::Color& color     = Core::Color::Ones() );


            inline EditablePrimitive() {}
            inline EditablePrimitive( const EditablePrimitive& other );
            inline EditablePrimitive& operator=( const EditablePrimitive& other );

            inline Type getType() const;
            inline const std::string& getName() const;

            /// Getters
            inline const Scalar&           asScalar() const;
            inline       Scalar&           asScalar();
            inline const Core::Vector3&    asPosition() const;
            inline       Core::Vector3&    asPosition();
            inline const Core::Quaternion& asRotation() const;
            inline       Core::Quaternion& asRotation();
            inline const Core::Vector3&    asScale() const;
            inline       Core::Vector3&    asScale();
            inline const Core::Color&      asColor() const;
            inline       Core::Color&      asColor();


        private:

            Variant m_value;
            Type m_type;
            std::string m_name;


        };

        /// Interface for all objects that can be edited through their properties.
        class EditableInterface
        {
        public:
            /// Get a list of all editable properties.
            virtual void getProperties( Core::AlignedStdVector<EditablePrimitive>& propsOut ) const = 0;

            /// Change the value of one property.
            virtual void setProperty( const EditablePrimitive& prop ) = 0;
        };

    }
}

#include <Engine/Entity/EditablePrimitive.inl>

#endif //RADIUMENGINE_EDITABLE_PRIMITIVE_HPP_