#pragma once

class Dirtyable;

class WashingMachine {
public:
    WashingMachine() = delete;

    static void add(Dirtyable* dirtyable);
    static void start();
};

/**
/// \brief The Dirtyable class
/// The Dirtyable class could be a parent class of all objects having
/// data on the cpu side (model) and another representation on the gpu
/// side (view). This class allows our client to prevent the gpu data
/// is dirty/outdated with the aid of the setDirty method.
/// So before rendering an image our renderer will be able to update all
/// the objects that have been modified by the client.
/// To do this we ask the user to specify how to update the data on the
/// gpu side with the clean method.
/// That we will do at most once per image for each dirtyable object.
///
/// This class could be the Displayable class by making all objects that
/// have cpu and gpu data inherit from this class. The only drawback is
/// that the Displayable class seems to be very close to the visible
/// objects in the scene but a sampler is not visible in the scene as an object.
/// For example pure virtual functions of Displayable like getAbsractGeometry,
/// render, getNumFaces, getNumeVertices don't make sense for a texture.
/// That's why I implemented a separate class.
**/
class Dirtyable {
public:
    virtual ~Dirtyable() = default;

    // implement this function updating data from model to view (cpu -> gpu).
    virtual void clean() = 0;

    // call this method on client side when the model data is updated.
    void setDirty();

private:
    bool m_dirty { true };

    friend class WashingMachine;
};
