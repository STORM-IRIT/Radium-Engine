#pragma once

namespace Ra {
namespace Core {
namespace Utils {

/// \brief This class defines the introspection interface a container need to implement.
///
/// This API allows to parse and copy contained element, or send/map to GPU buffer, given a generic
/// pointer to the container. Typical usage are shown in Ra::Core::Utils::Attrib or
/// Ra::Core::Geometry::IndexedGeometry This "interface" do not contain any data.
class ContainerIntrospectionInterface
{
  public:
    /// only needed for correct abstract dtor call.
    virtual ~ContainerIntrospectionInterface() {}
    /// \return the number of element (i.e. faces, vector) contained in the array.
    virtual size_t getSize() const = 0;

    /// Return the number of components of one element (i.e. for glVertexAttribPointer size argument
    /// ) \see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
    /// \return the number of components of one element (i.e. faces, vector)
    virtual size_t getNumberOfComponents() const = 0;

    /// \return the size in byte of container
    virtual size_t getBufferSize() const = 0;

    /// \return the stride, in bytes, from one attribute address to the next one.
    /// \warning it's meaningful only if the attrib do not contain heap allocated data. Such attrib
    /// could not be easily sent to the GPU, and getStride is meaningless.
    virtual int getStride() const = 0;

    /// \return a const pointer to the raw data
    virtual const void* dataPtr() const = 0;
};

} // namespace Utils
} // namespace Core
} // namespace Ra
