#ifndef RADIUMENGINE_HANDLE_SYSTEM
#define RADIUMENGINE_HANDLE_SYSTEM

#include <Core/Animation/Pose.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * Type for transform names
 */
using Label = std::string;

/**
 * The HandleArray class describes the base handle structure used for deformation.
 */
class RA_CORE_API HandleArray
{
  public:
    /**
     * The SpaceType specifies the space the transform lives in.
     */
    enum class SpaceType { LOCAL, MODEL };

    HandleArray();
    explicit HandleArray( const uint n );
    HandleArray( const HandleArray& handle ) = default;
    HandleArray& operator=( const HandleArray& handle ) = default;

    virtual ~HandleArray();

    /**
     * @return the number of transformations
     */
    inline virtual uint size() const;

    /**
     * Clear the transformations data
     */
    virtual void clear();

    /**
     * @return the current pose in the \p MODE space.
     */
    virtual const Pose& getPose( const SpaceType MODE ) const;

    /**
     * Set the current pose.
     * @param pose the new pose
     * @param MODE the space the psoe lives in
     */
    virtual void setPose( const Pose& pose, const SpaceType MODE );

    /**
     * @return the \p i-th transformation in \p MODE space.
     */
    virtual const Transform& getTransform( const uint i, const SpaceType MODE ) const;

    /**
     * Set the i-th transform to \p T, which is given in the MODE space.
     */
    virtual void setTransform( const uint i, const Transform& T, const SpaceType MODE );

    /**
     * @return the Handle's name
     */
    inline Label getName() const;

    /**
     * Set the Handle's name
     */
    inline void setName( const Label& name );

    /**
     * @return the name of the \p i-th transform.
     */
    inline Label getLabel( const uint i ) const;

    /**
     * Set the name of the \p i-th transform.
     */
    inline void setLabel( const uint i, const Label& text );

    /**
     * The current pose of the Handle
     */
    LocalPose m_pose;

  protected:
    /**
     * The Handle's name
     */
    Label m_name;

    /**
     * The names of the transforms.
     */
    std::vector<Label> m_label;
};

} // namespace Animation
} // Namespace Core
} // Namespace Ra

#include <Core/Animation/HandleArray.inl>

#endif // RADIUMENGINE_HANDLE_SYSTEM
