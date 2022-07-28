#pragma once

#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
namespace Data {

class Synchronizable;

///////////////////////////////////////////////
////            Synchronizer                ///
///////////////////////////////////////////////

namespace Synchronizer {

RA_ENGINE_API void start();

} // namespace Synchronizer

///////////////////////////////////////////////
////           Synchronizable               ///
///////////////////////////////////////////////

/**
/// \brief The Synchronizable class
/// The Synchronizable class could be a parent class of all objects having
/// data on the cpu side (model) and another representation on the gpu
/// side (view). This class allows our client to prevent the gpu data
/// is outdated with the aid of the needSync method.
/// So before rendering an image our renderer will be able to update/synchronize all
/// the objects that have been modified by the client.
/// To do this we ask the user to specify how to update the data on the
/// gpu side with the updateGL method.
/// That we will do at most once per image for each synchronizable object.
**/
class RA_ENGINE_API Synchronizable
{
  public:
    virtual ~Synchronizable();

    ///
    /// \brief updateGL
    /// Update data from model to view (cpu -> gpu).
    virtual void updateGL() = 0;

    ///
    /// \brief needSync
    /// Call this method on client side when the model data is updated.
    void needSync();
};

} // namespace Data
} // namespace Engine
} // namespace Ra
