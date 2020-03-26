#include <Core/Animation/Sequence.hpp>

namespace Ra {
namespace Core {
namespace Animation {

/// CONSTRUCTOR
Sequence::Sequence() : m_idx(), m_type( PoseType::Pose_RELATIVE ) {}
Sequence::Sequence( const Sequence& sequence ) :
    m_idx( sequence.m_idx ), m_type( sequence.m_type ) {}

/// DESTRUCTOR
Sequence::~Sequence() {}

/// INTERFACE
void Sequence::clear() {}
void Sequence::setSequence( const FrameSet& /*set*/ ) {}
void Sequence::setFrame( const int /*i*/, const Frame& /*frame*/ ) {}
void Sequence::insertFrame( const Frame& /*frame*/, const int /*i*/ ) {}
void Sequence::removeFrame( const int /*i*/ ) {}

} // namespace Animation
} // namespace Core
} // namespace Ra
