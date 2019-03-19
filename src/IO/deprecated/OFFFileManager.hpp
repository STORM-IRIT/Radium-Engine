#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/Geometry/TriangleMesh.hpp>
#include <IO/RaIO.hpp>
#include <IO/deprecated/FileManager.hpp>

namespace Ra {
namespace IO {

/**
 * The class OFFFileManager handles the loading and storing of TriangleMesh in
 * the standard OFF file format.
 */
class[[deprecated]] RA_IO_API OFFFileManager : public FileManager<Core::Geometry::TriangleMesh> {
  public:
    OFFFileManager();

    ~OFFFileManager() override;

  protected:
    /**
     * Returns the expected file header.
     */
    std::string header() const;

    std::string fileExtension() const override;

    bool importData( std::istream & file, Core::Geometry::TriangleMesh & data ) override;

    bool exportData( std::ostream & file, const Core::Geometry::TriangleMesh& data ) override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
