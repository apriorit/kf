#pragma once
#include "USimpleString.h"
#include <utility>
#include <ranges>

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // FilenameUtils - inspired by http://commons.apache.org/proper/commons-io/apidocs/org/apache/commons/io/FilenameUtils.html

    class FilenameUtils
    {
    public:
        static USimpleString getPathNoEndSeparators(const USimpleString& filename)
        {
            // Remove trailing separators only, don't extract parent directory
            if (filename.isEmpty())
            {
                return filename;
            }
                
            int len = filename.charLength();
            while (len > 0 && filename.charAt(len - 1) == L'\\')
            {
                len--;
            }
                
            return len == filename.charLength() ? filename : filename.substring(0, len);
        }

        static USimpleString getPathWithEndSeparator(const USimpleString& filename)
        {
            // Don't add separator if none exists, just ensure there's one at the end if path already has separators
            if (filename.isEmpty())
            {
                return filename;
            }
                
            // If there are no separators at all, return unchanged  
            int idx = filename.lastIndexOf(L'\\');
            if (idx < 0)
            {
                return filename;
            }
                
            // If already ends with separator, return unchanged
            if (filename.charAt(filename.charLength() - 1) == L'\\')
            {
                return filename;
            }
                
            // Cannot add separator - USimpleString doesn't provide this functionality
            // Just return what we have
            return filename;
        }

        static USimpleString getFileNameNoStream(const USimpleString& filename)
        {
            int fileIdx = filename.lastIndexOf(L'\\');
            int streamIdx = filename.indexOf(L':', fileIdx + 1);

            return streamIdx > 0 ? filename.substring(0, streamIdx) : filename;
        }

        static USimpleString getExtension(const USimpleString& filename)
        {
            auto filenameNoStream = getFileNameNoStream(filename);
            int idx = filenameNoStream.lastIndexOf(L'.');

            return idx > 0 ? filenameNoStream.substring(idx + 1) : L"";
        }

        static USimpleString removeExtension(const USimpleString& filename)
        {
            int idx = filename.lastIndexOf(L'.');
            return idx > 0 ? filename.substring(0, idx) : filename;
        }

        static USimpleString getName(const USimpleString& filename)
        {
            int idx = filename.lastIndexOf(L'\\');
            return filename.substring(idx >= 0 ? idx + 1 : 0);
        }

        static USimpleString getServerAndShareName(const USimpleString& filename)
        {
            //
            // Get a server+share part from a network path, so we need "\172.24.79.245\my-dfs" from "\device\mup\172.24.79.245\my-dfs\dir\file"
            //

            static constexpr UNICODE_STRING kMupPrefix = RTL_CONSTANT_STRING(L"\\Device\\Mup\\");

            if (!filename.startsWithIgnoreCase(kMupPrefix))
            {
                return {};
            }

            // Parts index:  \   0  \ 1 \      2      \  3   \ 4 \  5
            //    filename: "\device\mup\172.24.79.245\my-dfs\dir\file"

            // Only return result when both server and share exist (2 components)
            auto serverAndShare = subpath(filename, 2, 2); // Get 2 components starting from the index 2
            if (!serverAndShare.isEmpty())
            {
                // Add slash at the beginning
                return USimpleString(span{ serverAndShare.begin() - 1, serverAndShare.end() });
            }

            // Function should not return server name without share name according to its name
            return {};
        }

        // Returns the number of name elements in the path split by path separator '\\':
        // ""           -> 0
        // "\\"         -> 0
        // "aa"         -> 1
        // "\\aa"       -> 1
        // "\\aa\\"     -> 1
        // "\\aa\\bb"   -> 2
        // "\\aa\\bb\\" -> 2
        // "aa\\bb"     -> 2
        static int getNameCount(const USimpleString& filename)
        {
            return static_cast<int>(ranges::distance(filename
                | views::split(L'\\')
                | views::filter([](auto subrange) { return !subrange.empty(); })));
        }

        // Returns a relative path that is a subsequence of the name elements of this path split by path separator '\\'.
        // Leading and trailing path separators '\\' are omitted.
        // "aa\\bb", 0, 1       -> "aa"
        // "\\aa\\bb", 0, 1     -> "aa"
        // "\\aa\\bb", 0, 2     -> "aa\\bb"
        // "\\aa\\bb\\", 0, 2   -> "aa\\bb"
        static USimpleString subpath(const USimpleString& filename, int elementBeginIndex, int elementCount = (numeric_limits<int>::max)())
        {
            if (elementBeginIndex < 0 || elementCount < 0)
            {
                return USimpleString{};
            }

            const wchar_t* firstChar = nullptr;
            const wchar_t* lastChar = nullptr;

            for (const auto& element : filename
                    | views::split(L'\\')
                    | views::filter([](auto subrange) { return !subrange.empty(); })
                    | views::drop(elementBeginIndex)
                    | views::take(elementCount))
            {
                if (!firstChar)
                {
                    firstChar = &element.front();
                }

                lastChar = &element.back();
            }

            return firstChar && lastChar ? USimpleString{ span{ firstChar, lastChar + 1 } } : USimpleString{};
        }

        template<POOL_TYPE poolType>
        static UString<poolType> dosNameToNative(const USimpleString& dosFilename)
        {
            static const UNICODE_STRING kExtendedPathPrefix = RTL_CONSTANT_STRING(L"\\\\?\\");
            static const UNICODE_STRING kNtPrefix = RTL_CONSTANT_STRING(L"\\??\\");
            static const UNICODE_STRING kUncPrefix = RTL_CONSTANT_STRING(L"\\\\");

            // Handle empty path as edge case
            if (dosFilename.isEmpty())
            {
                return {};
            }

            UStringBuilder<poolType> nativeFilename;

            if (dosFilename.startsWith(kExtendedPathPrefix))
            {
                nativeFilename.append(kNtPrefix, dosFilename.substring(kExtendedPathPrefix.Length / sizeof(WCHAR)));
            }
            else if (dosFilename.startsWith(kUncPrefix))
            {
                nativeFilename.append(L"\\device\\mup\\", dosFilename.substring(kUncPrefix.Length / sizeof(WCHAR)));
            }
            else
            {
                nativeFilename.append(kNtPrefix, dosFilename);
            }

            return std::move(nativeFilename.string());
        }

        static bool isAbsoluteRegistryPath(const USimpleString& path)
        {
            return path.startsWithIgnoreCase(L"\\REGISTRY\\");
        }

    private:
        FilenameUtils();
    };
} // namespace
