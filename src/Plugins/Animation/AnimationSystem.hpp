#ifndef ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
#define ANIMPLUGIN_ANIMATION_SYSTEM_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <Engine/Entity/System.hpp>

namespace AnimationPlugin
{
    class ANIM_PLUGIN_API AnimationSystem :  public Ra::Engine::System
    {
    public:
        virtual void generateTasks(Ra::Core::TaskQueue* taskQueue, const Ra::Engine::FrameInfo& frameInfo);
        void initialize( ) override { m_isPlaying = true; }
		void handleFileLoading(Ra::Engine::Entity* entity, const std::string &filename) override;
		void setPlaying(bool isPlaying);
    protected:
        virtual Ra::Engine::Component* addComponentToEntityInternal(Ra::Engine::Entity* entity, uint id);
	private:
		bool m_isPlaying;

    };

}


#endif // ANIMPLUGIN_ANIMATION_SYSTEM_HPP_
