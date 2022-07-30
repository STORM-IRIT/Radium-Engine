#pragma once

#include <Engine/RaEngine.hpp>

namespace Ra {
namespace Engine {
namespace Data {

class Synchronizable;

///////////////////////////////////////////////
////            Synchronizer                ///
///////////////////////////////////////////////

///
/// The synchronizer is a service that records all synchronizable objects that have been updated
/// on cpu side (model).
/// In our engine we run the synchronization once per rendered image.
/// The synchronization (start function) consists to update the visible data on gpu side (view)
/// for whom the model has been modified.
/// If the model has been modified several times before rendering the current image,
/// we will update only the most recent data (view).
///
namespace Synchronizer {

///
/// \brief start the synchronization process, consists of updating gpu state of each synchronizable
/// objects.
/// \pre be in the active gl context within the rendering thread to be able to call gl
/// functions.
///
RA_ENGINE_API void start();

} // namespace Synchronizer

///////////////////////////////////////////////
////           Synchronizable               ///
///////////////////////////////////////////////

/**
/// \brief The Synchronizable class allows a lazy update of the data visualization (view) when the
/// user data (model) is updated. Please use this class if your class has a need to be visualized
and
/// can be updated in a client side that does not require/have access to the rendering context.
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
    /// \brief updateGL function must be implemented by the user of this class, this implemented
    /// function must specify how to update your object from the cpu data (model) to the view data
    /// (view).
    /// Update data from model to view (cpu -> gpu).
    virtual void updateGL() = 0;

    ///
    /// \brief needSync function must be called by the user of this class when the cpu data is
    /// recently updated. Call this method on client side when the model data is updated.
    void needSync();
};

} // namespace Data
} // namespace Engine
} // namespace Ra
