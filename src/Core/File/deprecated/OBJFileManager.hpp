#ifndef RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#define RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

class RA_CORE_API OBJFileManager : public FileManager<TriangleMesh> {
  public:
    OBJFileManager();

    virtual ~OBJFileManager();

    std::string fileExtension() const override;
    bool importData( std::istream& file, TriangleMesh& data ) override;
    bool exportData( std::ostream& file, const TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OBJ_FILE_MANAGER_HPP
