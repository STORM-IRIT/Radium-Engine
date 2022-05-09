#include <Engine/Data/EnvironmentTexture.hpp>

namespace Ra {
namespace Engine {
namespace Data {

inline const std::string& EnvironmentTexture::getImageName() const {
    return m_name;
}
inline EnvironmentTexture::EnvMapType EnvironmentTexture::getImageType() const {
    return m_type;
}

inline void EnvironmentTexture::setSkybox( bool state ) {
    m_isSkyBox = state;
}

inline bool EnvironmentTexture::isSkybox() const {
    return m_isSkyBox;
}

} // namespace Data
} // namespace Engine
} // namespace Ra
