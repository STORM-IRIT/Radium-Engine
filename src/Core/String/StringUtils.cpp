#include <Core/String/StringUtils.hpp>

#include <Core/CoreMacros.hpp>

#ifdef OS_WINDOWS
#define NOMINMAX				// Avoid C2039 MSVC compiler error
#undef vsnprintf
#define vsnprintf(buffer, count, format, argptr) vsnprintf_s(buffer, count, count, format, argptr)
#endif

#include <algorithm> // std::min std::max
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <memory>
#include <sstream>

namespace Ra { namespace Core
{
    namespace StringUtils
    {
        std::string getFileExt(const std::string& str)
        {
            std::string res;
            ulong pos = str.find_last_of('.');
            res = pos < str.size() ? str.substr(pos) : "";
            return res;
        }


        std::string getDirName(const std::string& path)
        {
            // We remove any trailing slashes.
            ulong pos = path.find_last_not_of('/');

            // Don't strip the last / from "/"
            if (pos == std::string::npos)
            {
                pos = path.find_first_of("/");
            }

            std::string res;
            res = path.substr(0, pos + 1);

            // Now find the previous slash and cut the string.
            pos = res.find_last_of('/');

            // The directory is actually "/" because the last slash is in first position.
            // In that case we should return "/"
            if (pos == 0)
            {
                res = "/";
            }
            else if (pos != std::string::npos)
            {
                res = res.substr(0, pos);
            }
            else
            {
                res = ".";
            }

            return res;
        }

        std::string getBaseName(const std::string& path)
        {
            std::string res;
            // We remove any trailing slashes.
            ulong pos = path.find_last_not_of('/');
            // Don't strip the last / from "/"
            if (pos == std::string::npos)
            {
                pos = path.find_first_of("/");
            }

            res = path.substr(0, pos + 1);

            // Now find the previous slash and cut the string.
            pos = res.find_last_of('/');

            if (pos != std::string::npos)
            {
                res = res.substr(pos + 1);
            }

            return res;
        }


        int stringPrintf(std::string& str, const char* fmt, ...)
        {
            // Random guessing value from the size of the format string.
            size_t size = strlen(fmt) * 2;
            int finalSize = 0;
            str.clear();

            std::unique_ptr<char[]> buffer;
            va_list args;

            while (1)
            {
                // Dynamically allocate a string and assign it to the unique ptr
                buffer.reset(new char[size]);

                // Attempt to printf into the buffer
                va_start(args, fmt);
                finalSize = vsnprintf(&buffer[0], size, fmt, args);
                va_end(args);

                // If our buffer was too small, we know that final_size
                // gives us the required buffer size.
                if (finalSize >= size)
                {
                    size = std::max(size + 1, size_t(finalSize));
                }
                else
                {
                    break;
                }
            }
            if (finalSize > 0)
            {
                str = std::string(buffer.get());
            }
            return finalSize;
        }

        int appendPrintf(std::string& str, const char* fmt, ...)
        {
            std::string toAppend;
            va_list args;
            va_start(args, fmt);
            int result = stringPrintf(toAppend, fmt, args);
            str += toAppend;
            va_end(args);
            return result;
        }

        std::vector<std::string> splitString(const std::string& str, char token)
        {
            std::stringstream ss(str);
            std::string item;
            std::vector<std::string> items;

            while (std::getline(ss, item, token))
            {
                items.push_back(item);
            }

            return items;
        }
    }
}}
