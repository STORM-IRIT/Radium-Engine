#ifndef RADIUMENGINE_FRAME_INFO_HPP_
#define RADIUMENGINE_FRAME_INFO_HPP_
namespace Ra { namespace Engine
{
    /// Structure passed to each system before they fill the task queue.
    struct FrameInfo
    {
        /// Time elapsed since the last frame in seconds.
        Scalar m_dt;

        // Other stuf (e.g. which systems are present, etc).
    };
}}
#endif // RADIUMENGINE_FRAME_INFO_HPP_
