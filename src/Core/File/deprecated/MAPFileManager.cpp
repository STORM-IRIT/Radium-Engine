#include <Core/File/deprecated/MAPFileManager.hpp>

namespace Ra {
namespace Core {

using namespace Geometry;

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
MAPFileManager::MAPFileManager() : FileManager< Parametrization >() { }



/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
MAPFileManager::~MAPFileManager() { }



/// ===============================================================================
/// HEADER
/// ===============================================================================
std::string MAPFileManager::header() const {
    return "MAP";
}



/// ===============================================================================
/// INTERFACE
/// ===============================================================================
std::string MAPFileManager::fileExtension() const {
    return "map";
}



bool MAPFileManager::importData( std::istream& file, Parametrization& data ) {
    std::string h;
    uint        size;
    file >> h;
    if( h != header() ) {
        addLogErrorEntry( "HEADER IS NOT CORRECT." );
        return false;
    }
    file >> size;
    if( size == 0 ) {
        addLogErrorEntry( "MAPPING ROW SIZE IS ZERO." );
        return false;
    }
    data = Parametrization( size, Mapping() );
    for( uint i = 0; i < size; ++i ) {
        uint ID;
        Scalar alpha;
        Scalar beta;
        Scalar delta;
        file >> ID >> alpha >> beta >> delta;
        data[i].setID( Index( ID ) );
        data[i].setAlpha( alpha );
        data[i].setBeta( beta );
        data[i].setDelta( delta );
    }
    return true;
}



bool MAPFileManager::exportData( std::ostream& file, const Parametrization& data ) {
    std::string content = "";
    const uint size = data.size();
    if( size == 0 ) {
        addLogErrorEntry( "MAPPING IS EMPTY." );
        return false;
    }
    content += header() + " " + std::to_string( size ) + "\n";
    for( uint i = 0; i < size; ++i ) {
        content += std::to_string( data[i].getID().getValue() ) + " " +
                   std::to_string( data[i].getAlpha()         ) + " " +
                   std::to_string( data[i].getBeta()          ) + " " +
                   std::to_string( data[i].getDelta()         ) + "\n";
    }
    file << content;
    return true;
}



} // namespace Core
} // namespace Ra
