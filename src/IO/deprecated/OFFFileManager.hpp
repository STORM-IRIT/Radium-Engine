#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/Geometry/TriangleMesh.hpp>
#include <IO/RaIO.hpp>
#include <IO/deprecated/FileManager.hpp>

namespace Ra {
namespace IO {

/*
 * The class OFFFileManager handles the loading and storing of TriangleMesh in the standard OFF
 * format.
 */
class RA_IO_API[[deprecated]] OFFFileManager : public FileManager<Core::Geometry::TriangleMesh> {
  public:
    /// CONSTRUCTOR
    OFFFileManager();

    /// DESTRUCTOR
    ~OFFFileManager() override;

  protected:
    /// HEADER
    std::string header() const;

    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::istream & file, Core::Geometry::TriangleMesh & data ) override;
    virtual bool exportData( std::ostream & file, const Core::Geometry::TriangleMesh& data )
        override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
