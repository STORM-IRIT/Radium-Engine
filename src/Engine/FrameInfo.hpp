#ifndef RADIUMENGINE_FRAME_INFO_HPP
#define RADIUMENGINE_FRAME_INFO_HPP

#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
/// Structure passed to each system before they fill the task queue.
struct RA_ENGINE_API FrameInfo {
    /// Time elapsed since the last frame in seconds.
    Scalar m_dt;

    /// Number of frames since the start of the application.
    // (NB : at 60 FPS on a 32 bits machine this will cycle every two years... ;) )
    uint m_numFrame;

    // Other stuff (e.g. which systems are present, etc).
};
} // namespace Engine
} // namespace Ra
#endif // RADIUMENGINE_FRAME_INFO_HPP_
