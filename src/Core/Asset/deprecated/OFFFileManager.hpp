#ifndef RADIUMENGINE_OFF_FILE_MANAGER_HPP
#define RADIUMENGINE_OFF_FILE_MANAGER_HPP

#include <Core/Asset/deprecated/FileManager.hpp>
#include <Core/Geometry/TriangleMesh.hpp>

namespace Ra {
namespace Core {
namespace Asset {
namespace deprecated {


/*
 * The class OFFFileManager handles the loading and storing of TriangleMesh in the standard OFF
 * format.
 */
class OFFFileManager : public FileManager<Geometry::TriangleMesh> {
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
    virtual bool importData( std::istream& file, Geometry::TriangleMesh& data ) override;
    virtual bool exportData( std::ostream& file, const Geometry::TriangleMesh& data ) override;
};

} // namespace deprecated
} // namespace Asset
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_OFF_FILE_MANAGER_HPP
