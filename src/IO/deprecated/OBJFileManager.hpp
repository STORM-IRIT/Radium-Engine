#ifndef RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#define RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#include <Core/Geometry/TriangleMesh.hpp>
#include <IO/RaIO.hpp>
#include <IO/deprecated/FileManager.hpp>

namespace Ra {
namespace IO {

/**
 * The class OBJFileManager handles the loading and storing of TriangleMesh in
 * the standard OBJ file format.
 */
class [[deprecated]] RA_IO_API OBJFileManager : public FileManager<Core::Geometry::TriangleMesh> {
  public:
    OBJFileManager();

    ~OBJFileManager() override;

  protected:
    std::string fileExtension() const override;

    bool importData( std::istream & file, Core::Geometry::TriangleMesh & data ) override;

    bool exportData( std::ostream & file, const Core::Geometry::TriangleMesh& data ) override;
};

} // namespace IO
} // namespace Ra

#endif // RADIUMENGINE_OBJ_FILE_MANAGER_HPP
