\page develanimation API: Animation pipeline
[TOC]
# Animation in Radium

The Radium Engine provides a set of classes dedicated to animation,
which can be found in `Core/Animation`.

In Radium, the animation of any data can be done through the `Ra::Core::Animation::KeyFramedValue`
template. It stores the data keyframes, i.e. time-value couples, which are
interpolated when the value for a given time is requested.

## Character Animation Basics

In order to animate a digital character, one may choose from two main animation techniques:
 - skeleton-based animation, in which the character's mesh is bound to an
animation skeleton composed of joints, sometimes called bones.
 - cage-based animation, in which the character's mesh is bound to a set of
points defining an englobing deformation cage.

In both cases, the character's mesh is bound to the handles (skeleton joints or
cage points) through a set of skinning weights.
During the animation, the handles transformations are first updated with respect
to the animation pose, then the mesh is deformed by combining the handles
transformations through the skinnning weights.

## Character Animation in Radium

The Radium Engine provides the basic classes for character animation.

Starting from a `Ra::Core::Animation::HandleArray`, which represents a deformation metaphor, such as `Ra::Core::Animation::Cage` or `Ra::Core::Animation::Skeleton`,
one must associate influence weights to each vertex of a mesh within a `Ra::Core::Animation::WeightMatrix`
(rows representing vertices, columns handles).
These influence weights, or skinning weights, are then used by a skinning algorithm to deform
the associated mesh vertices w.r.t. the handles deformation defined by the animation pose.
The mesh is also linked to each handle through a `bindMatrix` that expresses the
transformation from the mesh's local frame to the handle local frame.

In the case of character animation, an animation is a set of `Ra::Core::Animation::KeyFramedValues<Ra::Core::Transform>`,
one for each joint, which are interpolated by default as follows:
 - the translation and scale channels are linearly interpolated;
 - the rotation channel is linearly interpolated in quaternion space.

Animations are stored in local space, i.e. each handle transformation is expressed
in the handle's frame (transformation from bone's parent for skeleton bones,
position in cage's frame for cage points).

Hence, in order to deform the character's mesh, one must first use the handle's `bindMatrix`
to express the mesh vertices position into the handles' local space before combining the
handles local transformations to deform the mesh vertices.

## The Skeleton-Based Character Animation Plugin

The `SkeletonBasedAnimation` plugin (https://gitlab.com/Storm-IRIT/radium-official-plugins/skeletonbasedanimation)
provides one plugin specific to skeleton-based character animation and skinning.
This plugin provides a `SkeletonComponent`, which manages user interactions with
the animation skeleton, enabling posing the character and playing the animations.
It also provides a `SkinningComponent`, which is responsible for deforming the
object's mesh vertices according to the desired skinning method.

## Importing skeleton-based character animation data into Radium

In order to import animation related data into Radium, the default loader would be the `Ra::IO::AssimpLoader`,
 which deals with several standard animation formats (fbx, collada, ...).
In case one need to develop his own loader because using a file format not managed by the
Assimp library, the Radium Engine provides a set of classes in `Core/Asset` defining data
wrappers filled by loaders (see the specific documentation -- which does not exist for now).
For now, `Ra::Core::Asset::FileLoaderInterface`s in the Radium Engine produce only one `Ra::Core::Asset::FileData` per loaded file.

Regarding character animation data, there are a few things to do in order to ensure correct import by the
animation and skinning plugins:
 * The generated `Ra::Core::Asset::FileData` must contain exactly one `Ra::Core::Asset::HandleData` per animated object, as much `Ra::Core::Asset::AnimationData`
   as needed per animated object and may contain several or no `Ra::Core::Asset::GeometryData`.
   Note: for objects composed of several mesh parts, these must be referred by their name when filling the `Ra::Core::Asset::HandleData`.
 * The animation skeleton, i.e. the bones and hierarchy, must be loaded into the
   `Ra::Core::Asset::HandleData`, as well as the skinning weights.
   The bone hierarchy will be defined by the `Ra::Core::Asset::EdgeData`.
   Each bone will be represented by a `Ra::Core::Asset::HandleComponentData`, storing the bone's name,
   transform in model space and its per-skinned mesh offset matrix and vertex skinning weights.
   The offset matrix stores the transformation from mesh space to the node's local space.
 * Animation keyframes, i.e. couples of time and pose (in local space), if any, must be loaded into an `Ra::Core::Asset::AnimationData`.
   The whole animation must be stored bone-wise in `Ra::Core::Asset::HandleAnimation`s, whose name must match the bone's.
