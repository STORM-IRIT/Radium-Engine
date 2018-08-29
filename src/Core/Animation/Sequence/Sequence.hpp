#ifndef RADIUMENGINE_SEQUENCE_HPP
#define RADIUMENGINE_SEQUENCE_HPP

#include <Core/Animation/Pose/Pose.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/Index/CircularIndex.hpp>
#include <vector>

namespace Ra {
namespace Core {
namespace Animation {

using Frame = Pose;
using FrameSet = std::vector<Frame>;

/**
 * The class Sequence is a container for an animation sequence.
 *
 **/
class Sequence {
  public:
    /// Specify if the sequence stores trasformations to be applied or
    /// poses to be assigned.
    enum class PoseType { Pose_RELATIVE, Pose_ABSOLUTE };

    Sequence();

    Sequence( const Sequence& sequence );

    virtual ~Sequence();

    /// Return the size of the sequence.
    virtual uint size() const = 0;

    /// Clear the sequence. By default does nothing.
    virtual void clear();

    /// Return true if the size of the sequence is equal to zero.
    inline bool isEmpty() const;

    /// Return the type of the stored poses.
    inline PoseType getPoseType() const;

    /// Set the type of the stored poses.
    inline void setPoseType( const PoseType& type );

    /// Return the set of frames.
    virtual FrameSet getSequence() const = 0;

    /// Set the frame set.
    virtual void setSequence( const FrameSet& set );

    /// Get the i-th frame.
    virtual Frame frame( const int i ) const = 0;

    /// Set the i-th frame to \p frame.
    virtual void setFrame( const int i, const Frame& frame );

    /// Return the frame in the next position.
    inline Frame nextFrame() const;

    /// Return the frame in the prev position.
    inline Frame prevFrame() const;

    /// Return the frame in the current position.
    inline Frame currentFrame() const;

    /// Return the frame in the first position.
    inline Frame firstFrame() const;

    /// Return the frame in the last position.
    inline Frame lastFrame() const;

    /// Return the index of the next frame.
    inline uint nextFrameIndex() const;

    /// Return the index of the prev frame.
    inline uint prevFrameIndex() const;

    /// Return the index of the current frame.
    inline uint currentFrameIndex() const;

    /// Return the index of the first frame.
    inline uint firstFrameIndex() const;

    /// Return the index of the last frame.
    inline uint lastFrameIndex() const;

    /// Insert a frame at the i-th position.
    virtual void insertFrame( const Frame& frame, const int i );

    /// Insert a frame at the next position.
    inline void insertNext( const Frame& frame );

    /// Insert a frame at the prev position.
    inline void insertPrev( const Frame& frame );

    /// Insert a frame at the current position.
    inline void insertCurrent( const Frame& frame );

    /// Insert a frame at the first position.
    inline void insertFirst( const Frame& frame );

    /// Insert a frame at the last position.
    inline void insertLast( const Frame& frame );

    /// Remove the frame at the i-th position.
    virtual void removeFrame( const int i );

    /// Remove the frame at the next position.
    inline void removeNext();

    /// Remove the frame at the prev position.
    inline void removePrev();

    /// Remove the frame at the current position.
    inline void removeCurrent();

    /// Remove the frame at the first position.
    inline void removeFirst();

    /// Remove the frame at the last position.
    inline void removeLast();

    /// Move to frame i.
    inline void moveToFrame( const int i );

    /// Move to next frame.
    inline void moveToNextFrame();

    /// Move to prev frame.
    inline void moveToPrevFrame();

    /// Move to first frame.
    inline void moveToFirstFrame();

    /// Move to last frame.
    inline void moveToLastFrame();

    /// Reset the sequence.
    inline void reset();

  private:
    /// Index of the Current Frame.
    CircularIndex m_idx;

    /// Type of the stored poses.
    PoseType m_type;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_SEQUENCE_HPP
