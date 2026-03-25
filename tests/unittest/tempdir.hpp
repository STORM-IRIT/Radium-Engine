// TempDir
// SPDX-FileCopyrightText: 2024-present Benno Waldhauer
// SPDX-License-Identifier: MIT

#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <iostream>
#include <random>
#include <string>

namespace bw::tempdir
{
namespace fs = std::filesystem;

// Default log implementation used by Config struct
inline void log(const std::string& message) { std::cout << message << std::endl; }

// Exception class for errors related to TempDir operations.
// E.g. for issues encountered during the creation, usage, or deletion of temporary directories.
// It wraps another exception to retain the original error message.
class TempDirException : public std::runtime_error
{
  public:
    TempDirException(const std::exception& ex)
        : std::runtime_error(std::string("TempDirExcepton: ") + ex.what())
    {
    }
};

//  enum of cleanup modes for TempDir
//  When a scope with a temporary directory completes, it might be
//  useful in some cases to be able to view the contents of the temporary directory used by the
//  scope. Cleanup allows you to control how a TempDir is cleaned up.
enum class Cleanup
{
    always,     // Always clean up a temporary directory after TempDir goes out of scope.
    on_success, // Clean up a temporary directory after TempDir goes out of scope
                // without uncaught exceptions.
    never       // Never clean up a temporary directory after TempDir goes out of scope.
};

// struct holding configuration options for TempDir
// It allows to specify the root path of temporary directory, the cleanup and logging behavior
// as well as the temporary directory prefix.
struct Config
{
    fs::path root_path = fs::temp_directory_path();
    Cleanup cleanup = Cleanup::always;
    std::string temp_dir_prefix = "temp_dir";
    std::function<void(const std::string&)> log_impl;

    Config& set_root_path(const fs::path& root_path)
    {
        this->root_path = root_path;
        return *this;
    }

    Config& set_cleanup(Cleanup cleanup)
    {
        this->cleanup = cleanup;
        return *this;
    }

    Config& set_temp_dir_prefix(const std::string& temp_dir_prefix)
    {
        this->temp_dir_prefix = temp_dir_prefix;
        return *this;
    }

    Config& enable_logging()
    {
        this->log_impl = bw::tempdir::log;
        return *this;
    }

    Config& enable_logging(std::function<void(const std::string&)> log_impl)
    {
        this->log_impl = log_impl;
        return *this;
    }
};

// TempDir manages temporary directories with automatic cleanup based on user-defined policies.
//
// The TempDir class is designed to simplify the creation and management of temporary directories.
// It supports automatic cleanup based on configurable policies (e.g., always cleanup, cleanup only
// on successful execution, or never cleanup). The class ensures proper handling of errors during
// directory creation and cleanup, and on demand logs relevant messages for each operation.
//
// The directory is created in a user-specified or default root path and given a unique name
// generated based on a timestamp and a random number. Errors related to directory operations
// are wrapped in TempDirException.
//
// Cleanup will be automatically handled based on the configured policy when the TempDir object goes
// out of scope or when cleanup method is called explicitly
class TempDir
{
  public:
    // Constructs a TempDir with a specified root path
    // where the temporary directory will be created.
    explicit TempDir(fs::path root_path) : TempDir(Config().set_root_path(root_path)) {}

    //  Constructs a TempDir with a specified root path and cleanup policy.
    //  root_path: The root path where the temporary directory will be created.
    //  cleanup: The cleanup policy to apply when the object is destroyed.
    explicit TempDir(fs::path root_path, Cleanup cleanup)
        : TempDir(Config().set_root_path(root_path).set_cleanup(cleanup))
    {
    }

    // Constructs a TempDir with a specified cleanup policy
    // which will be applied when the object is destroyed.
    explicit TempDir(Cleanup cleanup) : TempDir(Config().set_cleanup(cleanup)) {}

    // Constructs a TempDir with a fully specified configuration for the TempDir,
    // including path, prefix, cleanup policy, and logging.
    // If an error occurs during construction, a TempDirException is thrown.
    explicit TempDir(Config config = {}) : _config(config)
    {
        try
        {
            _temp_dir = config.root_path / generate_dir_name();
            fs::create_directories(_temp_dir);
            log("TempDir create '" + _temp_dir.string() + "'");
        }
        catch (const std::exception& ex)
        {
            log("TempDir creation of '" + _temp_dir.string() + "' failed. Error: " + ex.what());
            throw TempDirException(ex);
        }
    }

    // Destructor that handles automatic cleanup based on the configured policy.
    //
    // Attempts to clean up the temporary directory if the cleanup policy allows it.
    // Errors during cleanup are logged but not rethrown
    ~TempDir()
    {
        try
        {
            cleanup();
        }
        catch (const std::exception& e)
        {
            // do nothing as rethrowing is not allowed in destructor
            // error was already logged in cleanup method
        }
    }

    // Copying TempDir is disabled
    TempDir(const TempDir&) = delete;
    TempDir& operator=(const TempDir&) = delete;

    // Moving TempDir is enabeld
    TempDir(TempDir&&) = default;
    TempDir& operator=(TempDir&&) = default;

    // Returns the path of the managed temporary directory.
    const std::filesystem::path& path() const { return _temp_dir; }

    // Manually triggers cleanup of the temporary directory.
    // Attempts to delete the directory and its contents based on the configured
    // cleanup policy. If an error occurs during cleanup, a TempDirException is thrown.
    void cleanup()
    {
        if (!fs::exists(_temp_dir))
            return;

        bool always_cleanup = _config.cleanup == Cleanup::always;
        bool on_sucess_cleanup_no_exception =
            _config.cleanup == Cleanup::on_success && std::uncaught_exceptions() <= 0;

        if (!always_cleanup && !on_sucess_cleanup_no_exception)
        {
            log("TempDir keep '" + _temp_dir.string() + "'");
            return;
        }

        try
        {
            fs::remove_all(_temp_dir);
            log("TempDir remove '" + _temp_dir.string() + "'");
        }
        catch (const std::exception& ex)
        {
            log("TempDir removal of '" + _temp_dir.string() + "' failed. Error: " + ex.what());
            throw TempDirException(ex);
        }
    }

  private:
    // Generates a unique name for the temporary directory.
    std::string generate_dir_name()
    {
        using namespace std::chrono;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(10000, 99999);
        int random_number = dist(gen);

        auto now = system_clock::now();
        auto timestamp = duration_cast<milliseconds>(now.time_since_epoch()).count();

        std::string ts = std::to_string(timestamp);
        std::string rn = std::to_string(random_number);
        return _config.temp_dir_prefix + "_" + ts + "_" + rn;
    }

    // Logs a message using the configured logging implementation.
    void log(const std::string& message)
    {
        if (_config.log_impl)
            _config.log_impl(message);
    }

    std::filesystem::path _temp_dir;
    Config _config;
};

} // namespace bw::tempdir
