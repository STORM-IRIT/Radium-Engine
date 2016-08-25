#include <Core/Utils/File/FileManager.hpp>

namespace Ra {
namespace Core {

/// ===============================================================================
/// CONSTRUCTOR
/// ===============================================================================
template < typename DATA, bool Binary >
inline FileManager< DATA, Binary >::FileManager() : m_log("") { }



/// ===============================================================================
/// DESTRUCTOR
/// ===============================================================================
template < typename DATA, bool Binary >
inline FileManager< DATA, Binary >::~FileManager() { }



/// ===============================================================================
/// INTERFACE
/// ===============================================================================
template < typename DATA, bool Binary >
inline bool FileManager< DATA, Binary >::load( const std::string& filename,
                                       DATA&              data,
                                       const bool         SAVE_LOG_FILE ) {
    resetLog();
    addLogEntry( "Filename        : " + filename );
    addLogEntry( "Expected Format : " + fileExtension() );
    addLogEntry( "File Type       : " + std::string( Binary ? "Binary" : "Text" ) );
    addLogEntry( "Loading start..." );
    std::ifstream file( filename, std::ios_base::in | ( Binary ? std::ios_base::binary : std::ios_base::in ) );
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



template < typename DATA, bool Binary >
inline bool FileManager< DATA, Binary >::save( const std::string& filename,
                                       const DATA&        data,
                                       const bool         SAVE_LOG_FILE ) {
    resetLog();
    addLogEntry( "Filename         : " + filename );
    addLogEntry( "Exporting Format : " + fileExtension() );
    addLogEntry( "File Type        : " + std::string( Binary ? "Binary" : "Text" ) );
    addLogEntry( "Saving start..." );
    std::ofstream file( filename + "." + fileExtension(), std::ios_base::out | std::ios_base::trunc | ( Binary ? std::ios_base::binary : std::ios_base::out )  );
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
template < typename DATA, bool Binary >
inline std::string FileManager< DATA, Binary >::log() const {
    return m_log;
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::addLogEntry( const std::string& text ) {
    addLogEntry( text, NORMAL );
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::addLogWarningEntry( const std::string& text ) {
    addLogEntry( text, WARNING );
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::addLogErrorEntry( const std::string& text ) {
    addLogEntry( text, ERROR );
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::addLogEntry( const std::string& text, const LogEntryType type ) {
    switch ( type ) {
        case NORMAL: {
            m_log += text;
        } break;

        case WARNING: {
            m_log += "\n--- WARNING : " + text + " ---\n";
        } break;

        case ERROR: {
            m_log += "\n### ERROR : " + text + " ###\n";
        } break;

        default: break;
    }

    m_log += "\n";
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::resetLog() {
    m_log = "====== FILE MANAGER LOG ======\n";
}



template < typename DATA, bool Binary >
inline void FileManager< DATA, Binary >::saveLog( const std::string& filename ) {
    std::ofstream file( filename + ".log" );
    if( !file.is_open() ) {
        return;
    }
    file << log();
    file.close();
}


} // namespace Core
} // namespace Ra
