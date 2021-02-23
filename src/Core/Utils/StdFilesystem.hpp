#pragma once

#ifndef CXX_FILESYSTEM_HAVE_FS
#    error std::filesystem is required to compile this file
#endif
#if CXX_FILESYSTEM_IS_EXPERIMENTAL
#    include <experimental/filesystem>

// We need the alias from std::experimental::filesystem to std::filesystem
namespace std {
namespace filesystem = experimental::filesystem;
}

#else
#    include <filesystem>
#endif
