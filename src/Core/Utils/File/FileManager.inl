#include <Core/Utils/File/FileManager.hpp>

namespace Ra {
namespace Core {

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
template < typename DATA >
inline FileManager< DATA >::FileManager() : m_log("") { }



/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
template < typename DATA >
inline FileManager< DATA >::~FileManager() { }



/// ===============================================================================
/// INTERFACE
/// ===============================================================================
template < typename DATA >
inline bool FileManager< DATA >::load( const std::string& filename,
                                       DATA&              data,
                                       const bool         SAVE_LOG_FILE ) {
    resetLog();
    addLogEntry( "Filename        : " + filename );
    addLogEntry( "Expected Format : " + fileExtension() );
    addLogEntry( "Loading start..." );
    std::ifstream file( filename );
    if( !file.is_open() ) {
        addLogEntry( "Error occured while opening the file. HINT: FILENAME MAY BE WRONG." );
        return false;
    }
    addLogEntry( "File opened successfully." );
    addLogEntry( "Starting to import the data." );
    const bool status = importData( file, data );
    addLogEntry( "Import " + ( ( status ) ? std::string( "DONE." ) : std::string( "FAILED." ) ) );
    file.close();
    addLogEntry( "File closed." );
    addLogEntry( "Loading " + filename + " ended." );
    if( SAVE_LOG_FILE ) {
        saveLog( filename );
    }
    return status;
}



template < typename DATA >
inline bool FileManager< DATA >::save( const std::string& filename,
                                       const DATA&        data,
                                       const bool         SAVE_LOG_FILE ) {
    resetLog();
    addLogEntry( "Filename         : " + filename );
    addLogEntry( "Exporting Format : " + fileExtension() );
    addLogEntry( "Saving start..." );
    std::ofstream file( filename + "." + fileExtension() );
    if( !file.is_open() ) {
        addLogEntry( "Error occured while opening the file." );
        return false;
    }
    addLogEntry( "File opened successfully." );
    addLogEntry( "Starting to export the data..." );
    const bool status = exportData( file, data );
    addLogEntry( "Export " + ( ( status ) ? std::string( "DONE." ) : std::string( "FAILED." ) ) );
    file.close();
    addLogEntry( "File closed." );
    addLogEntry( "Saving " + filename + " ended." );
    if( SAVE_LOG_FILE ) {
        saveLog( filename );
    }
    return status;
}



/// ===============================================================================
/// LOG
/// ===============================================================================
template < typename DATA >
inline std::string FileManager< DATA >::log() const {
    return m_log;
}



template < typename DATA >
inline void FileManager< DATA >::addLogEntry( const std::string& text ) {
    m_log += text + "\n";
}



template < typename DATA >
inline void FileManager< DATA >::resetLog() {
    m_log = "====== FILE MANAGER LOG ======\n";
}



template < typename DATA >
inline void FileManager< DATA >::saveLog( const std::string& filename ) {
    std::ofstream file( filename + ".log" );
    if( !file.is_open() ) {
        return;
    }
    file << log();
    file.close();
}


} // namespace Core
} // namespace Ra
