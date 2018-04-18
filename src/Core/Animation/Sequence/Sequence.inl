#include <Core/Animation/Sequence/Sequence.hpp>

namespace Ra {
namespace Core {
namespace Animation {

inline bool Sequence::isEmpty() const {
    return ( size() == 0 );
}

inline Sequence::PoseType Sequence::getFrameType() const {
    return m_type;
}
inline void Sequence::setFrameType( const FrameType& type ) {
    m_type = type;
}

inline Frame Sequence::nextFrame() const {
    return pose( idx + 1 );
}
inline Frame Sequence::prevFrame() const {
    return pose( idx - 1 );
}
inline Frame Sequence::currentFrame() const {
    return pose( idx );
}
inline Frame Sequence::firstFrame() const {
    return pose( 0 );
}
inline Frame Sequence::lastFrame() const {
    return pose( size() - 1 );
}

inline uint Sequence::nextFrameIndex() const {
    return ( idx + 1 );
}
inline uint Sequence::prevFrameIndex() const {
    return ( idx - 1 );
}
inline uint Sequence::currentFrameIndex() const {
    return ( idx );
}
inline uint Sequence::firstFrameIndex() const {
    return ( 0 );
}
inline uint Sequence::lastFrameIndex() const {
    return ( size() - 1 );
}

inline void Sequence::insertNext( const Frame& frame ) {
    insertFrame( frame, ( idx + 1 ) );
}
inline void Sequence::insertPrev( const Frame& frame ) {
    insertFrame( frame, ( idx - 1 ) );
}
inline void Sequence::insertCurrent( const Frame& frame ) {
    insertFrame( frame, ( idx ) );
}
inline void Sequence::insertFirst( const Frame& frame ) {
    insertFrame( frame, 0 );
}
inline void Sequence::insertLast( const Frame& frame ) {
    insertFrame( frame, ( size() - 1 ) );
}

inline void Sequence::removeNext() {
    removeFrame( idx + 1 );
}
inline void Sequence::removePrev() {
    removeFrame( idx - 1 );
}
inline void Sequence::removeCurrent() {
    removeFrame( idx );
}
inline void Sequence::removeFirst() {
    removeFrame( 0 );
}
inline void Sequence::removeLast() {
    removeFrame( size() - 1 );
}

inline void Sequence::moveToFrame( const int i ) {
    idx.set_value( i );
}
inline void Sequence::moveToNextFrame() {
    moveTo( idx + 1 );
}
inline void Sequence::moveToPrevFrame() {
    moveTo( idx - 1 );
}
inline void Sequence::moveToFirstFrame() {
    moveTo( 0 );
}
inline void Sequence::moveToLastFrame() {
    moveTo( size() - 1 );
}
inline void Sequence::reset() {
    moveToFirst();
}

} // namespace Animation
} // namespace Core
} // namespace Ra
