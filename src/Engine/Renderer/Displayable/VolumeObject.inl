#include <Engine/Renderer/Displayable/VolumeObject.hpp>

namespace Ra {
namespace Engine {

const Core::Geometry::AbstractGeometry& VolumeObject::getAbstractGeometry() const {
    CORE_ASSERT( m_volume, "Volume is not initialized" );
    return *(Core::Geometry::AbstractGeometry*)( m_volume.get() );
}

Core::Geometry::AbstractGeometry& VolumeObject::getAbstractGeometry() {
    CORE_ASSERT( m_volume, "Volume is not initialized" );
    return *(Core::Geometry::AbstractGeometry*)( m_volume.get() );
}

/// Returns the underlying AbstractVolume
const Core::Geometry::AbstractVolume& VolumeObject::getVolume() const {
    CORE_ASSERT( m_volume, "Volume is not initialized" );
    return *m_volume.get();
}

Core::Geometry::AbstractVolume& VolumeObject::getVolume() {
    CORE_ASSERT( m_volume, "Volume is not initialized" );
    return *m_volume.get();
}

} // namespace Engine
} // namespace Ra
