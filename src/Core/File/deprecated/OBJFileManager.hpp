#ifndef RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#define RADIUMENGINE_OBJ_FILE_MANAGER_HPP
#include <Core/RaCore.hpp>
#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

class RA_CORE_API OBJFileManager : public FileManager< TriangleMesh > {
public:
    /// CONSTRUCTOR
    OBJFileManager();

    /// DESTRUCTOR
    virtual ~OBJFileManager();

    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::istream& file, TriangleMesh& data ) override;
    virtual bool exportData( std::ostream& file, const TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OBJ_FILE_MANAGER_HPP
