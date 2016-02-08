#ifndef SKINPLUGIN_SKINNING_COMPONENT_HPP_
#define SKINPLUGIN_SKINNING_COMPONENT_HPP_

#include <SkinningPlugin.hpp>

#include <Core/Animation/Handle/HandleWeight.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Engine/Assets/HandleData.hpp>
#include <Engine/Entity/Component.hpp>


namespace SkinningPlugin
{
    class SKIN_PLUGIN_API SkinningComponent : public Ra::Engine::Component
    {
    public:
        SkinningComponent( const std::string& name) : Component(name) {}
        virtual ~SkinningComponent() {}

        virtual void initialize() override {}

        void skin() ;

        virtual void handleWeightsLoading( const Ra::Asset::HandleData* data );

        virtual void getProperties(Ra::Core::AlignedStdVector<Ra::Engine::EditableProperty> &propsOut) const override {}
        virtual void setProperty(const Ra::Engine::EditableProperty &newProp) override {}
    private:

            std::string m_contentsName;
            Ra::Core::TriangleMesh m_referenceMesh;
    };
}

#endif //  SKINPLUGIN_SKINNING_COMPONENT_HPP_

