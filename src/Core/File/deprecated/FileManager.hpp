#ifndef RADIUMENGINE_FILE_MANAGER_HPP
#define RADIUMENGINE_FILE_MANAGER_HPP

#include <fstream>
#include <string>

namespace Ra {
namespace Core {

/**
 * The class FileManager gives the base common interface for handling loading/storing data from/to
 * files.
 */
template <typename DATA, bool Binary = false>
class FileManager {
  public:
    /// Used for indicating the type of message.
    enum LogEntryType {
        LogEntry_Normal,
        LogEntry_Warning,
        LogEntry_Error,
    };

    inline FileManager();

    virtual ~FileManager();

    /// Try to load file \p filename and convert its content into \p data.
    /// Return true if the data is correctly loaded, false otherwise.
    inline bool load( const std::string& filename, DATA& data, const bool SAVE_LOG_FILE = false );

    /// Try to save the content of \p data into the file \p filename.
    /// Return true if the data is correctly stored, false otherwise.
    inline bool save( const std::string& filename, const DATA& data,
                      const bool SAVE_LOG_FILE = false );

    /// Return the log of the lattest performed action.
    inline std::string log() const;

  protected:
    /// Add a normal line in the log file. A newline character will be automatically added.
    inline void addLogEntry( const std::string& text );

    /// Add a warning line in the log file. A newline character will be automatically added.
    inline void addLogWarningEntry( const std::string& text );

    /// Add an error line in the log file. A newline character will be automatically added.
    inline void addLogErrorEntry( const std::string& text );

    /// Add a line of type \p type in the log file. A newline character will be automatically added.
    inline void addLogEntry( const std::string& text, const LogEntryType type );

    /// Return the extension given to the files.
    virtual std::string fileExtension() const = 0;

    /// Load data from a given file. Return false if an error occurs, true otherwise.
    virtual bool importData( std::istream& file, DATA& data ) = 0;

    /// Store given data into a given file. Return false if an error occurs, true otherwise.
    virtual bool exportData( std::ostream& file, const DATA& data ) = 0;

  private:
    /// Reset the log string.
    inline void resetLog();

    /// Save the log into a text file.
    inline void saveLog( const std::string& filename );

    /// The log content.
    std::string m_log;
};

} // namespace Core
} // namespace Ra

#include <Core/File/deprecated/FileManager.inl>

#endif // RADIUMENGINE_FILE_MANAGER_HPP
