#ifndef RADIUMENGINE_SEQUENCE_HPP
#define RADIUMENGINE_SEQUENCE_HPP

#include <Core/Animation/Pose.hpp>
#include <Core/CoreMacros.hpp>
#include <Core/Container/CircularIndex.hpp>
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
    /// ENUM
    // Specify if the sequence stores trasformations to be applied or
    // poses to be assigned.
    enum class PoseType { Pose_RELATIVE, Pose_ABSOLUTE };

    /// CONSTRUCTOR
    Sequence();                           // Default constructor
    Sequence( const Sequence& sequence ); // Copy constructor

    /// DESTRUCTOR
    virtual ~Sequence();

    /// SIZE INTERFACE
    virtual uint size() const = 0; // Return the size of the sequence
    virtual void clear();          // Clear the sequence. By default does nothing

    /// QUERY
    inline bool isEmpty() const; // Return true if the size of the sequence is equal to zero

    /// POSE TYPE
    inline PoseType getPoseType() const;             // Get the type of the poses stored
    inline void setPoseType( const PoseType& type ); // Set the type of the poses stored

    /// SEQUENCE INTERFACE
    virtual FrameSet getSequence() const = 0;        // Return the set of frames
    virtual void setSequence( const FrameSet& set ); // Set the frame set

    /// POSE INTERFACE
    virtual Frame frame( const int i ) const = 0;             // Get the i-th frame.
    virtual void setFrame( const int i, const Frame& frame ); // Set the i-th frame to frame

    inline Frame nextFrame() const;    // Return the frame in the next position
    inline Frame prevFrame() const;    // Return the frame in the prev position
    inline Frame currentFrame() const; // Return the frame in the current position
    inline Frame firstFrame() const;   // Return the frame in the first position
    inline Frame lastFrame() const;    // Return the frame in the last position

    /// INDEX
    inline uint nextFrameIndex() const;    // Return the index of the next frame
    inline uint prevFrameIndex() const;    // Return the index of the prev frame
    inline uint currentFrameIndex() const; // Return the index of the current frame
    inline uint firstFrameIndex() const;   // Return the index of the first frame
    inline uint lastFrameIndex() const;    // Return the index of the last frame

    /// INSERT POSE
    virtual void insertFrame( const Frame& frame,
                              const int i );         // Insert a frame at the i-th position
    inline void insertNext( const Frame& frame );    // Insert a frame at the next position
    inline void insertPrev( const Frame& frame );    // Insert a frame at the prev position
    inline void insertCurrent( const Frame& frame ); // Insert a frame at the current position
    inline void insertFirst( const Frame& frame );   // Insert a frame at the first position
    inline void insertLast( const Frame& frame );    // Insert a frame at the last position

    /// REMOVE POSE
    virtual void removeFrame( const int i ); // Remove the frame at the i-th position
    inline void removeNext();                // Remove the frame at the next position
    inline void removePrev();                // Remove the frame at the prev position
    inline void removeCurrent();             // Remove the frame at the current position
    inline void removeFirst();               // Remove the frame at the first position
    inline void removeLast();                // Remove the frame at the last position

    /// SEQUENCE FLOW
    inline void moveToFrame( const int i ); // Move to frame i
    inline void moveToNextFrame();          // Move to next frame
    inline void moveToPrevFrame();          // Move to prev frame
    inline void moveToFirstFrame();         // Move to first frame
    inline void moveToLastFrame();          // Move to last frame
    inline void reset();                    // Reset the sequence

  private:
    /// VARAIBLE
    Container::CircularIndex m_idx;
    PoseType m_type;
};

} // namespace Animation
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_SEQUENCE_HPP
