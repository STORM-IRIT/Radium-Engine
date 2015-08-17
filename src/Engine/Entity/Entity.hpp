#ifndef RADIUMENGINE_ENTITY_HPP
#define RADIUMENGINE_ENTITY_HPP

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>

#include <Core/Index/IndexedObject.hpp>
#include <Core/Index/Index.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Engine/Entity/EditableProperty.hpp>

namespace Ra
{
    namespace Engine
    {
        class Component;
    }
}

namespace Ra
{
    namespace Engine
    {

        class RA_API Entity : public Core::IndexedObject, public EditableInterface
        {
        public:
            RA_CORE_ALIGNED_NEW
            explicit Entity( const std::string& name = "" );
            ~Entity();

            inline const std::string& getName() const;
            inline void rename( const std::string& name );

            inline void setTransform( const Core::Transform& transform );
            inline void setTransform( const Core::Matrix4& transform );
            Core::Transform getTransform() const;
            Core::Matrix4 getTransformAsMatrix() const;

            void addComponent( Component* component );

            void removeComponent( const std::string& name );
            void removeComponent( Component* component );

            Component* getComponent( const std::string& name );
            const std::map<std::string, Engine::Component*>& getComponentsMap() const;


            // Editable Interface
            virtual void getProperties(Core::AlignedStdVector<EditableProperty>& entityPropsOut) const override;
            virtual void setProperty(const EditableProperty& prop) override;

        private:
            Core::Transform m_transform;

            std::string m_name;

            typedef std::pair<std::string, Engine::Component*> ComponentByName;
            std::map<std::string, Engine::Component*> m_components;

            mutable std::mutex m_transformMutex;
        };

    } // namespace Engine
} // namespace Ra

#include <Engine/Entity/Entity.inl>

#endif // RADIUMENGINE_ENTITY_HPP
