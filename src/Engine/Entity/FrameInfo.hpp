#ifndef RADIUMENGINE_FRAME_INFO_HPP
#define RADIUMENGINE_FRAME_INFO_HPP

#include <Core/CoreMacros.hpp>

namespace Ra
{
    namespace Engine
    {
        /// Structure passed to each system before they fill the task queue.
        struct RA_API FrameInfo
        {
            /// Time elapsed since the last frame in seconds.
            Scalar m_dt;

            // Other stuff (e.g. which systems are present, etc).
        };
    }
}
#endif // RADIUMENGINE_FRAME_INFO_HPP_
