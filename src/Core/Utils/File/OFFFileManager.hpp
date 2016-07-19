#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/Utils/File/FileManager.hpp>
#include <Core/Mesh/TriangleMesh.hpp>

namespace Ra {
namespace Core {

/*
* The class OFFFileManager handles the loading and storing of TriangleMesh in the standard OFF format.
*/
class OFFFileManager : public FileManager< TriangleMesh > {
public:
    /// CONSTRUCTOR
    OFFFileManager();

    /// DESTRUCTOR
    virtual ~OFFFileManager();

protected:
    /// HEADER
    std::string header() const;

    /// INTERFACE
    virtual std::string fileExtension() const override;
    virtual bool importData( std::ifstream& file, TriangleMesh& data ) override;
    virtual bool exportData( std::ofstream& file, const TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
