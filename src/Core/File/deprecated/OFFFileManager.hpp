#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/File/deprecated/FileManager.hpp>
#include <Core/Geometry/TriangleMesh.hpp>

namespace Ra {
namespace Core {

/*
 * The class OFFFileManager handles the loading and storing of TriangleMesh in the standard OFF
 * format.
 */
class OFFFileManager : public FileManager<Geometry::TriangleMesh> {
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
    virtual bool importData( std::istream& file, Geometry::TriangleMesh& data ) override;
    virtual bool exportData( std::ostream& file, const Geometry::TriangleMesh& data ) override;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
