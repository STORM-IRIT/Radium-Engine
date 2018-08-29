#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

/**
 * The class OFFFileManager handles the loading and storing of TriangleMesh in the standard OFF
 * format.
 */
class OFFFileManager : public FileManager<TriangleMesh> {
  public:
    OFFFileManager();

    virtual ~OFFFileManager();

  protected:
    /// Returns the expected file header.
    std::string header() const;

    std::string fileExtension() const override;
    bool importData( std::istream& file, TriangleMesh& data ) override;
    bool exportData( std::ostream& file, const TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
