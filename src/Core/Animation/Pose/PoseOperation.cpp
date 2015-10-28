#include <Core/Animation/Pose/PoseOperation.hpp>
#include <iostream>
#include <Eigen/Geometry>

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
    CORE_ASSERT( (a.size() == b.size()), "Poses are wrong");
    CORE_ASSERT( ( ( t >= 0.0 ) && ( t <= 1.0 ) ), "T is wrong");
    
    int size = a.size();
    Pose interpolatedPose;
    
    for (int i = 0; i < size; i++)
    {
        // interpolate between the transforms
        Ra::Core::Transform aTransform = a[i];
        Ra::Core::Transform bTransform = b[i];
        
        Ra::Core::Quaternion aRot = Ra::Core::Quaternion(aTransform.rotation());
        Ra::Core::Quaternion bRot = Ra::Core::Quaternion(bTransform.rotation());
        Ra::Core::Quaternion interpRot = aRot.slerp(t, bRot);
        
        Ra::Core::Vector3 interpTranslation = (1 - t) * aTransform.translation() + t * bTransform.translation();
        
        Ra::Core::Transform interpolatedTransform;
        interpolatedTransform.linear() = interpRot.toRotationMatrix();
        interpolatedTransform.translation() = interpTranslation;
        
        interpolatedPose.push_back(interpolatedTransform);
    }
    
    return interpolatedPose;
}

void interpolateTransforms(const Ra::Core::Transform& a, const Ra::Core::Transform& b, Scalar t, Ra::Core::Transform& interpolated)
{
    Ra::Core::Quaternion aRot = Ra::Core::Quaternion(a.rotation());
    Ra::Core::Quaternion bRot = Ra::Core::Quaternion(b.rotation());
    Ra::Core::Quaternion interpRot = aRot.slerp(t, bRot);
    
    Ra::Core::Vector3 interpTranslation = (1 - t) * a.translation() + t * b.translation();
    
    interpolated.linear() = interpRot.toRotationMatrix();
    interpolated.translation() = interpTranslation;
}

} // namespace Animation
} // namespace Core
} // namespace Ra
