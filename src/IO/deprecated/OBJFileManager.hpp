#pragma once
#include <Core/Geometry/TriangleMesh.hpp>
#include <IO/RaIO.hpp>
#include <IO/deprecated/FileManager.hpp>

namespace Ra {
namespace IO {

/** \warning will be removed once integration of file saving is done in new IO, see
 * https://github.com/STORM-IRIT/Radium-Engine/issues/362
 * \todo reimplement in IO
 * \deprecated see https://github.com/STORM-IRIT/Radium-Engine/issues/362
 */
class RA_IO_API OBJFileManager : public FileManager<Core::Geometry::TriangleMesh>
{
  public:
    /// CONSTRUCTOR
    OBJFileManager();

    /// DESTRUCTOR
    ~OBJFileManager() override;

    /// INTERFACE
    std::string fileExtension() const override;
    bool importData( std::istream& file, Core::Geometry::TriangleMesh& data ) override;
    bool exportData( std::ostream& file, const Core::Geometry::TriangleMesh& data ) override;
};

} // namespace IO
} // namespace Ra
