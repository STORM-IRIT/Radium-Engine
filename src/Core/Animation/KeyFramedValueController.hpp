#pragma once
#include <map>
#include <set>

#include <Core/Animation/KeyFramedValue.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/**
 * The KeyFrameController class provides a way to call callback functions on
 * KeyFramedValues upon keyframe insertion, modification or a modification of
 * the current time.
 */
class KeyFramedValueController
{
  public:
    using KeyFrame       = Ra::Core::Animation::KeyFramedValueBase;
    using InsertCallback = std::function<void( const Scalar& /*t*/ )>;
    using UpdateCallback = std::function<void( const Scalar& /*t*/ )>;

    KeyFramedValueController(
        KeyFrame* value         = nullptr,
        std::string&& name      = "__INVALID__",
        InsertCallback inserter = []( const Scalar& ) {},
        UpdateCallback updater  = []( const Scalar& ) {} ) :
        m_value( value ),
        m_name( std::move( name ) ),
        m_inserter( inserter ),
        m_updater( updater ) {}

    /**
     * Calls the InsertCallback to insert a keyframe at tie \p t.
     */
    void insertKeyFrame( Scalar t ) { m_inserter( t ); }

    /**
     * Calls the UpdateCallback to update the value at time t.
     */
    void updateKeyFrame( Scalar t ) { m_updater( t ); }

    /**
     * The KeyFramedValue to manage.
     */
    KeyFrame* m_value;

    /**
     * The name for the managed KeyFramedValue.
     */
    std::string m_name;

    /**
     * The callback fonction to call for keyframe insertion.
     */
    InsertCallback m_inserter;

    /**
     * The callback fonction to call for value update.
     */
    UpdateCallback m_updater;
};

} // namespace Animation
} // namespace Core
} // namespace Ra
