#include <Core/Animation/Pose/PoseOperation.hpp>

namespace Ra {
namespace Core {
namespace Animation {



bool compatible( const Pose& p0, const Pose& p1 ) {
    return ( p0.size() == p1.size() );
}



Pose relativePose( const Pose& modelPose, const RestPose& restPose )  {
    assert( compatible( modelPose, restPose ) );
    Pose T( restPose.size() );
    for( uint i = 0; i < T.size(); ++i ) {
        T[i] = modelPose[i] * restPose[i].inverse( Eigen::Affine );
    }
    return T;
}



Pose applyTransformation(const Pose& pose, const std::vector<Transform> &transform ) {
    Pose T( std::min( pose.size(), transform.size() ) );
    for( uint i = 0; i < T.size(); ++i ) {
        T[i] = transform[i] * pose[i];
    }
    return T;
}



Pose applyTransformation( const Pose& pose, const Transform& transform ) {
    Pose T( pose.size() );
    for( uint i = 0; i < T.size(); ++i ) {
        T[i] = transform * pose[i];
    }
    return T;
}


} // namespace Animation
} // namespace Core
} // namespace Ra
