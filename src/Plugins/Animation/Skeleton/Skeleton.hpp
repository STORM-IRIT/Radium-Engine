#ifndef ANIMPLUGIN_SKELETON_HPP_
#define ANIMPLUGIN_SKELETON_HPP_

#include <Plugins/Animation/AnimationPlugin.hpp>

#include <vector>
#include <string>

#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Containers/AlignedStdVector.hpp>

// Skeletons and poses.

// A skeleton is a tree where node are joints.

// A bone really is a link in space between joints. But by abuse of language
// a bone is usually identified with the joint linking it to its parent (called
// the proximal joint.
// If the bone is a leaf it represents a terminal point of the skeleton and
// has a length of 0.
// If the bone has children, it links it proximal joint with one or several
// other joints called distal joints. Thus one bone might be represented
// by one or several segments linking the proximal joint to the distal joints.

// Bones are indexed from 0 to num_bones.
// Bones have one parent and 0 to several children. Arrays store the parent
// and child relationships.
// A bone may have no parent, in this case its parent index is-1. Usually there
// is only one bone (the "root") which has no parent, but this skeleton class
// does not enforce it.
// "Leaf" bones represent the terminal points of the skeleton and have length 0

// A pose is a set of transforms, one for each bone.

// Poses are a set of transforms associated to a skeleton describing the bones
// attitude. Poses are usually given in model space. The ith transform
// maps model space to "bone space" which has its origin in the proximal joint.
// A pose can also be given in bone space (a.k.a. local space), in this case
// each transform is relative to its parent bone's transform. The Pose class
// keeps a version of the pose in both spaces.

// Finaly, a relative pose is a pose defined relatively to a reference pose.
// Of course a relative pose can be seen both in model space or local space.
// Utilities are provided in the Pose class to manipulate relative poses.


using std::vector;

/// A class representing a skeleton (directed acyclic graph of bones).
/// Each bone has one parent, except the root(s).
/// Structure is kept with the parents array which maps bone index i to
/// the index of its parent. Root bones have an index of -1.
/// The bones must be in hierarchical order, i.e. no bone can be
/// stored before its parent.
namespace AnimationPlugin
{
    /// A shortcut for an array of transforms which can be interpreted as a pose. @see Pose 
    /// for a more complete data structure to hold a pose.
    typedef Ra::Core::AlignedStdVector<Ra::Core::Transform> RawPose;
    
    /// A small structure keeping the informations of the bones.
    struct ANIM_PLUGIN_API Bone 
    {
        Bone(const std::string& name) : m_name(name) {}
        std::string m_name;
    };
    
    class ANIM_PLUGIN_API Skeleton 
    {
    public:
        // Skeleton building.

        /// Empty constructor
        Skeleton(const std::string& name);

        /// Appends a bone to the given skeleton. Parent index must be -1 or
        /// already exist in the skeleton.
        void addBone(const Bone& bone, int parent_idx);

        /// Sets the reference pose in model space.
        void setRefPose(const RawPose& pose);

        // Accessors.

        /// Returns the skeleton's name.
        const std::string& getName() const;

        /// Returns the number of bones.
        uint getNumBones() const;

        /// Returns the parent index of given bone (-1 if bone has no parent)
        int getParentIdx(int boneIdx) const;

        /// Access the bone at given index.
        const Bone& getBone(int boneIdx) const;

        /// Returns the ref pose (in model space).
        const RawPose& getRefPose() const;

        /// Returns the children of given bone.
        const std::vector<int>& getChildrenIdx(int boneIdx) const;

        /// Returns true if the given bone is a leaf bone.
        bool isLeaf(int boneIdx) const;

        // Debug checks.

        /// Debug consistency checks;
        void check() const;

    private:
        vector<Bone> m_bones;     /// storage for the bones
        vector<int> m_parents;   /// parent indices of bones.
        vector<vector<int> > m_children;  /// child indices of bones
        RawPose m_refPose;  /// reference pose in model space.
        std::string m_name;      /// Name of our rig

    };

}
#endif //ANIMPLUGIN_SKELETON_HPP_

