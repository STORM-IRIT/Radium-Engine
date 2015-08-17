#ifndef RADIUMENGINE_EDITABLE_PROPERTY_HPP_
#define RADIUMENGINE_EDITABLE_PROPERTY_HPP_

#include <Core/CoreMacros.hpp>

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
class EditableProperty
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
    static inline EditableProperty scalar  (const std::string& name, Scalar scalar = 0.f);
    static inline EditableProperty position(const std::string& name, const Core::Vector3& vector  = Core::Vector3::Zero());
    static inline EditableProperty rotation(const std::string& name, const Core::Quaternion& quat = Core::Quaternion::Identity());
    static inline EditableProperty scale   (const std::string& name, const Core::Vector3& scale   = Core::Vector3::Ones());
    static inline EditableProperty color   (const std::string& name, const Core::Color& color     = Core::Color::Ones());

    
    inline EditableProperty() {}
    inline EditableProperty(const EditableProperty& other);
    inline EditableProperty& operator=(const EditableProperty& other);

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
    virtual void getProperties(Core::AlignedStdVector<EditableProperty>& propsOut) const = 0;

    /// Change the value of one property.
    virtual void setProperty(const EditableProperty& prop) = 0;
};

}
}

#include <Engine/Entity/EditableProperty.inl>

#endif //RADIUMENGINE_EDITABLE_PROPERTY_HPP_