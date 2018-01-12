#ifndef RADIUMENGINE_HANDLE_INCLUDE
#define RADIUMENGINE_HANDLE_INCLUDE

#include <Core/RaCore.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Animation/Pose/Pose.hpp>

namespace Ra {
namespace Core {
namespace Animation {

typedef std::string Label;

/**
* The Handle class describe the base handle structure used for deformation.
*/
class RA_CORE_API Handle {
public:
    /// ENUM
    enum class SpaceType {
        LOCAL,
        MODEL
    };

    /// CONSTRUCTOR
    Handle();                       // Default constructor
    Handle( const uint n );         // Reserve the memory for n transforms
    Handle( const Handle& handle ) = default; // Copy constructor
    Handle& operator=( const Handle& handle ) = default; //Assignment operator

    /// DESTRUCTOR
    ~Handle();

    /// SIZE
    inline virtual uint size() const; // Return the number of transforms
    virtual void clear();             // Empty the pose

    /// SPACE INTERFACE
    virtual const Pose& getPose( const SpaceType MODE ) const;                                  // Return the pose in MODE space
    virtual void       setPose( const Pose& pose, const SpaceType MODE );                       // Set the MODE space pose
    virtual const Transform&  getTransform( const uint i, const SpaceType MODE ) const;         // Return the i-th transform matrix of the pose in MODE space
    virtual void       setTransform( const uint i, const Transform& T, const SpaceType MODE );  // Set the i-th transform of the MODE space pose

    /// NAME
    inline Label getName() const;                             // Return the label of the handle
    inline void  setName( const Label& name );                // Set the label of the handle
    inline Label getLabel( const uint i ) const;              // Return the label of the i-th transform
    inline void  setLabel( const uint i, const Label& text ); // Set the label of the i-th transform

    /// VARIABLE
    LocalPose m_pose; // The pose of the handle

protected:
    /// VARIABLE
    Label m_name;                   // Label of the handle
    std::vector< Label > m_label;   // Label of each transform
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#include <Core/Animation/Handle/Handle.inl>

#endif // RADIUMENGINE_HANDLE_INCLUDE

