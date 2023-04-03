#pragma once

#include <fstream>
#include <string>

namespace Ra {
namespace IO {

/**
 * The class FileManager gives the base common interface for handling loading/storing data from/to
 * files.
 * \warning will be removed once integration of file saving is done in new IO, see
 * https://github.com/STORM-IRIT/Radium-Engine/issues/362
 * \todo reimplement in IO
 * \deprecated see https://github.com/STORM-IRIT/Radium-Engine/issues/362
 */
template <typename DATA, bool Binary = false>
class FileManager
{
  public:
    /// ENUM
    enum LogEntryType {
        LogEntry_Normal,
        LogEntry_Warning,
        LogEntry_Error,
    };

    /// CONSTRUCTOR
    inline FileManager() = default; // Default constructor.

    /// DESTRUCTOR
    inline virtual ~FileManager() = default; // Destructor.

    /// INTERFACE
    inline bool load( const std::string& filename,
                      DATA& data,
                      const bool SAVE_LOG_FILE =
                          false ); // Return true if the data is correctly loaded, false otherwise.
    inline bool save( const std::string& filename,
                      const DATA& data,
                      const bool SAVE_LOG_FILE =
                          false ); // Return true if the data is correctly stored, false otherwise.

    /// LOG
    inline std::string log() const; // Return the log of the last action performed.

  protected:
    /// LOG
    inline void addLogEntry( const std::string& text ); // Add a line in the log file. A newline
                                                        // character will be automatically added.
    inline void
    addLogWarningEntry( const std::string& text );           // Add a warning line in the log file.
    inline void addLogErrorEntry( const std::string& text ); // Add an error line in the log file.
    inline void addLogEntry( const std::string& text, const LogEntryType type );

    /// INTERFACE
    virtual std::string fileExtension() const = 0; // Return the extension given to the files.
    virtual bool importData( std::istream& file,
                             DATA& data ) = 0; // Load data from a given file. Return false if an
                                               // error occurs, true otherwise.
    virtual bool exportData( std::ostream& file,
                             const DATA& data ) = 0; // Store given data into a given file. Return
                                                     // false if an error occurs, true otherwise.

  private:
    /// LOG
    inline void resetLog();                             // Reset the log string.
    inline void saveLog( const std::string& filename ); // Save the log into a text file.

    /// VARIABLE
    std::string m_log { "" };
};

} // namespace IO
} // namespace Ra

namespace Ra {
namespace IO {

//////////////////////////////////////////////////////////////////////////////
// INTERFACE
//////////////////////////////////////////////////////////////////////////////
template <typename DATA, bool Binary>
inline bool FileManager<DATA, Binary>::load( const std::string& filename,
                                             DATA& data,
                                             const bool SAVE_LOG_FILE ) {
    bool status = true;
    resetLog();
    addLogEntry( "Filename        : " + filename );
    addLogEntry( "Expected Format : " + fileExtension() );
    addLogEntry( "File Type       : " + std::string( Binary ? "Binary" : "Text" ) );
    addLogEntry( "Loading start..." );
    std::ifstream file(
        filename, std::ios_base::in | ( Binary ? std::ios_base::binary : std::ios_base::in ) );
    if ( !( status = file.is_open() ) ) {
        addLogWarningEntry(
            "Error occurred while opening the file. Trying to see if extension is missing..." );
        file.open( filename + "." + fileExtension(),
                   std::ios_base::in | ( Binary ? std::ios_base::binary : std::ios_base::in ) );
        if ( !( status = file.is_open() ) ) {
            addLogErrorEntry( "Error occured while opening the file. HINT: FILENAME IS WRONG." );
        }
    }
    if ( status ) {
        addLogEntry( "File opened successfully." );
        addLogEntry( "Starting to import the data." );
        status = importData( file, data );
        addLogEntry( "Import " + ( ( status ) ? std::string( "DONE." ) : std::string( "FAILED." ) ),
                     ( ( status ) ? LogEntry_Normal : LogEntry_Error ) );
        file.close();
        addLogEntry( "File closed." );
    }
    addLogEntry( "Loading " + filename + " ended." );
    if ( SAVE_LOG_FILE ) { saveLog( filename + "_load" ); }
    return status;
}

template <typename DATA, bool Binary>
inline bool FileManager<DATA, Binary>::save( const std::string& filename,
                                             const DATA& data,
                                             const bool SAVE_LOG_FILE ) {
    bool status = true;
    resetLog();
    addLogEntry( "Filename         : " + filename );
    addLogEntry( "Exporting Format : " + fileExtension() );
    addLogEntry( "File Type        : " + std::string( Binary ? "Binary" : "Text" ) );
    addLogEntry( "Saving start..." );
    std::ofstream file( filename + "." + fileExtension(),
                        std::ios_base::out | std::ios_base::trunc |
                            ( Binary ? std::ios_base::binary : std::ios_base::out ) );
    if ( !( status = file.is_open() ) ) {
        addLogErrorEntry( "Error occured while opening the file." );
    }
    if ( status ) {
        addLogEntry( "File opened successfully." );
        addLogEntry( "Starting to export the data..." );
        status = exportData( file, data );
        addLogEntry( "Export " + ( ( status ) ? std::string( "DONE." ) : std::string( "FAILED." ) ),
                     ( ( status ) ? LogEntry_Normal : LogEntry_Error ) );
        file.close();
        addLogEntry( "File closed." );
    }
    addLogEntry( "Saving " + filename + " ended." );
    if ( SAVE_LOG_FILE ) { saveLog( filename + "_save" ); }
    return status;
}

//////////////////////////////////////////////////////////////////////////////
// LOG
//////////////////////////////////////////////////////////////////////////////
template <typename DATA, bool Binary>
inline std::string FileManager<DATA, Binary>::log() const {
    return m_log;
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::addLogEntry( const std::string& text ) {
    addLogEntry( text, LogEntry_Normal );
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::addLogWarningEntry( const std::string& text ) {
    addLogEntry( text, LogEntry_Warning );
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::addLogErrorEntry( const std::string& text ) {
    addLogEntry( text, LogEntry_Error );
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::addLogEntry( const std::string& text,
                                                    const LogEntryType type ) {
    switch ( type ) {
    case LogEntry_Normal: {
        m_log += text;
    } break;

    case LogEntry_Warning: {
        m_log += "\n--- LogEntry_Warning : " + text + " ---\n";
    } break;

    case LogEntry_Error: {
        m_log += "\n### LogEntry_Error : " + text + " ###\n";
    } break;

    default:
        break;
    }

    m_log += "\n";
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::resetLog() {
    m_log = "====== FILE MANAGER LOG ======\n";
}

template <typename DATA, bool Binary>
inline void FileManager<DATA, Binary>::saveLog( const std::string& filename ) {
    std::ofstream file( filename + ".log" );
    if ( !file.is_open() ) { return; }
    file << log();
    file.close();
}

} // namespace IO
} // namespace Ra
