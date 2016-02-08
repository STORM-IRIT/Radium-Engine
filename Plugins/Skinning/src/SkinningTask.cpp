#include <SkinningTask.hpp>
#include <SkinningComponent.hpp>

void SkinningPlugin::SkinnerTask::process()
{
   m_component->skin();
}
