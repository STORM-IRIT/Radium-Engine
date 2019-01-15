#ifndef RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#define RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Geometry/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

class RA_CORE_API OBJFileManager : public FileManager<Geometry::TriangleMesh> {
  public:
    /// CONSTRUCTOR
    OBJFileManager();

    /// DESTRUCTOR
    ~OBJFileManager() override;

    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::istream& file, Geometry::TriangleMesh& data ) override;
    virtual bool exportData( std::ostream& file, const Geometry::TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OBJ_FILE_MANAGER_HPP
