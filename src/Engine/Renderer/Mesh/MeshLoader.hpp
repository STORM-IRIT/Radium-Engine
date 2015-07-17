#ifndef RADIUMENGINE_MESHLOADER_HPP
#define RADIUMENGINE_MESHLOADER_HPP

#include <string>

namespace Ra { namespace Engine { class RadiumEngine; } }

namespace Ra { namespace Engine {

namespace MeshLoader
{
    void loadFile(const std::string& name, RadiumEngine* engine);
} // namespace MeshLoader;

} // namespace Engine
} // namespace Ra

#endif
