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

Pose interpolatePoses(const Pose& a, const Pose& b, Scalar t)
{
    assert(a.size() == b.size());
    
    int size = a.size();
    Pose interpolatedPose;
    
    for (int i = 0; i < size; i++)
    {
        // interpolate between the transforms
        Ra::Core::Transform aTransform = a[i];
        Ra::Core::Transform bTransform = b[i];
        
        Ra::Core::Quaternion aRot = Ra::Core::Quaternion(aTransform.rotation());
        Ra::Core::Quaternion bRot = Ra::Core::Quaternion(bTransform.rotation());
        Ra::Core::Quaternion interpRotation = aRot.slerp(t, bRot);
        
        Ra::Core::Vector3 interpTranslation = (1 - t) * aTransform.translation() + t * bTransform.translation();
        
        Ra::Core::Transform interpolatedTransform;
        interpolatedTransform.linear() = interpRotation.toRotationMatrix();
        interpolatedTransform.translation() = interpTranslation;
        
        interpolatedPose.push_back(interpolatedTransform);
    }
    
    return interpolatedPose;
}

} // namespace Animation
} // namespace Core
} // namespace Ra
