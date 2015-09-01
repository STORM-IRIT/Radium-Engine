#ifndef RADIUMENGINE_EDITABLE_PROPERTY_HPP_
#define RADIUMENGINE_EDITABLE_PROPERTY_HPP_

#include <Engine/RaEngine.hpp>

#include <Engine/Entity/EditablePrimitive.hpp>

namespace Ra
{
    namespace Engine
    {
        /// A property is a set of primitives that must be packed and edited together.
        /// Among them some might be read-only and some read-write.
        struct EditableProperty
        {
            struct Entry
            {
                Entry(const EditablePrimitive& prim, bool canEdit = true)
                        : primitive(prim), isEditable(canEdit){}
                Entry (const Entry& ) = default;
                Entry& operator= (const Entry&) = default;

                EditablePrimitive primitive;
                bool isEditable;
            };

            enum Type
            {
                PRIMITIVE,  // Exactly one primitive.
                ARRAY,      // An array of primitives of similar type
                TRANSFORM,  // Special case for transform : a rotation (primitive 0) and a translation (primitive 1).
                VARIANTS,   // A generic array of different typed properties.
            };

            EditableProperty(Type t, const std::string& name) : type(t), name(name) {}
            EditableProperty(const EditableProperty&) = default;
            EditableProperty& operator= (const EditableProperty&) = default;

            // special constructor from transform.
            EditableProperty( const Core::Transform& tr, const std::string& name, bool canEditTrans = true, bool canEditRot = true)
            : type(TRANSFORM), name(name)
            {
                primitives.push_back(Entry(EditablePrimitive::position( "Position", tr.translation()), canEditTrans));
                primitives.push_back(Entry(EditablePrimitive::rotation( "Rotation", Core::Quaternion(tr.rotation())),canEditRot));
            }

            std::vector<Entry> primitives;
            Type type;
            std::string name;
        };

        /// Interface for all objects that can be edited through their properties.
        class EditableInterface
        {
        public:
            /// Get a list of all editable properties.
            virtual void getProperties( Core::AlignedStdVector<EditableProperty>& propsOut ) const = 0;

            /// Change the value of one property.
            virtual void setProperty( const EditableProperty& prop ) = 0;
        };

    }
}

#endif //RADIUMENGINE_EDITABLE_PROPERTY_HPP_
