#include <IO/AsciiPCLoader/asciipcloader.hpp>

#include <Core/Asset/FileData.hpp>
#include <Core/Utils/Attribs.hpp> //logs
#include <fstream>
#include <iostream>

using std::ifstream;
using std::make_unique;
using std::runtime_error;
using std::to_string;

const string trajExt( "txt" );

namespace Ra {
namespace IO {

vector<string> AsciiPointCloudLoader::getFileExtensions() const {
    return vector<string>( { "*." + trajExt } );
}

bool AsciiPointCloudLoader::handleFileExtension( const string& extension ) const {
    return extension.compare( trajExt ) == 0;
}

Ra::Core::Asset::FileData* AsciiPointCloudLoader::loadFile( const string& filename ) {
    using Ra::Core::Asset::FileData;
    using Ra::Core::Asset::GeometryData;
    using Ra::Core::Utils::logERROR;
    using Ra::Core::Utils::logINFO;

    FileData* fileData = new Ra::Core::Asset::FileData( filename );

    if ( !fileData->isInitialized() ) {
        delete fileData;
        LOG( logINFO ) << "Filedata could not be initialized";
        return nullptr;
    }

    auto startTime = clock();
    // a unique name is required by the component messaging system
    static int id = 0;
    auto geometry =
        make_unique<GeometryData>( "TRAJ_PC_" + to_string( ++id ), GeometryData::POINT_CLOUD );
    if ( geometry == nullptr ) {
        LOG( logERROR ) << "could not create geometry";
        return nullptr;
    }

    geometry->setFrame( Ra::Core::Transform::Identity() );

    ifstream stream( filename );
    if ( !stream.is_open() ) {
        throw runtime_error( "Could not open ifstream to path " + filename );
        return nullptr;
    }
    else {
        LOG( logINFO ) << "---Beginning file loading---";
    }

    /*reading how data is arranged*/
    string line;
    vector<unsigned long> spacepos;
    unsigned long timepos, xpos, ypos, zpos;

    if ( !getline( stream, line ) ) {
        delete fileData;
        LOG( logINFO ) << "file does not contain trajectory data format. aborting";
        return nullptr;
    }

    // Find mandatory fields
    timepos = line.find( "Time" );
    xpos    = line.find( " X" );
    ypos    = line.find( " Y" );
    zpos    = line.find( " Z" );

    if ( timepos == string::npos ) {
        delete fileData;
        LOG( logINFO ) << "file does not contain time property. aborting";
        return nullptr;
    }

    if ( xpos == string::npos ) {
        delete fileData;
        LOG( logINFO ) << "file does not contain X property. aborting";
        return nullptr;
    }

    if ( ypos == string::npos ) {
        delete fileData;
        LOG( logINFO ) << "file does not contain Y property. aborting";
        return nullptr;
    }

    if ( zpos == string::npos ) {
        delete fileData;
        LOG( logINFO ) << "file does not contain Z property. aborting";
        return nullptr;
    }

    LOG( logINFO ) << "Loading properties \"x\", \"y\", \"z\", \"time\".";

    // skipping blank space
    xpos++;
    ypos++;
    zpos++;

    /*retrieving x y z time attribut positions and column count*/
    int spacecount = 0;
    for ( size_t i = 0; i < line.length(); ++i ) {
        if ( line[i] == ' ' ) {
            spacepos.emplace_back( i );
            spacecount++;
        }
    }

    for ( int i = 0; i < spacecount; ++i ) {
        if ( timepos <= spacepos[i] ) {
            timepos = i;
            break;
        }
    }

    for ( int i = 0; i < spacecount; ++i ) {
        if ( xpos <= spacepos[i] ) {
            xpos = i;
            break;
        }
    }

    for ( int i = 0; i < spacecount; ++i ) {
        if ( ypos <= spacepos[i] ) {
            ypos = i;
            break;
        }
    }

    for ( int i = 0; i < spacecount; ++i ) {
        if ( zpos <= spacepos[i] ) {
            zpos = i;
            break;
        }
    }

    vector<double> pointRecord;

    auto& vertices = geometry->getGeometry().verticesWithLock();

    // creating custom attrib for time record
    auto handle     = geometry->getGeometry().vertexAttribs().addAttrib<Scalar>( "time" );
    auto& attrib    = geometry->getGeometry().vertexAttribs().getAttrib( handle );
    auto& container = attrib.getDataWithLock();

    /*recording data*/
    int i = 0;
    double num;
    while ( stream >> num ) {
        pointRecord.emplace_back( num );

        // retrieving attributes from their position in line
        if ( i == spacecount ) {
            vertices.emplace_back( Scalar( pointRecord[xpos] ),
                                   Scalar( pointRecord[ypos] ),
                                   Scalar( pointRecord[zpos] ) );
            container.emplace_back( Scalar( pointRecord[timepos] ) );

            pointRecord.clear();
        }

        i = ( i + 1 ) % ( spacecount + 1 );
    }

    stream.close();

    geometry->getGeometry().verticesUnlock();
    geometry->getGeometry().vertexAttribs().unlock( handle );

    // finalizing
    fileData->m_geometryData.clear();
    fileData->m_geometryData.reserve( 1 );
    fileData->m_geometryData.push_back( move( geometry ) );

    fileData->m_loadingTime = ( clock() - startTime ) / Scalar( CLOCKS_PER_SEC );

    LOG( logINFO ) << "---File loading ended---";

    fileData->displayInfo();

    fileData->m_processed = true;

    return fileData;
}

string AsciiPointCloudLoader::name() const {
    return "Trajectory";
}

} // namespace IO
} // namespace Ra
