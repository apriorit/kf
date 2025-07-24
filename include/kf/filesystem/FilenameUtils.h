#pragma once
#include <utility>
#include "kf/string/USimpleString.h"

namespace kf
{
    // TODO: Implement Path type instead of utils
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // FilenameUtils - inspired by http://commons.apache.org/proper/commons-io/apidocs/org/apache/commons/io/FilenameUtils.html

    class FilenameUtils
    {
    public:
        static USimpleString getPathNoEndSeparator(const USimpleString& filename)
        {
            int idx = filename.lastIndexOf(L'\\');
            return filename.substring(0, idx);
        }

        static USimpleString getPathWithEndSeparator(const USimpleString& filename)
        {
            int idx = filename.lastIndexOf(L'\\');
            return filename.substring(0, idx + 1);
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
            return filename.substring(idx > 0 ? idx + 1 : 0);
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

            auto serverAndShare = subpath(filename, 2, 2); // Get 2 components starting from the index 2
            if (serverAndShare.isEmpty())
            {
                return {};
            }

            // Add slash at the beginning
            return USimpleString(span{ serverAndShare.begin() - 1, serverAndShare.end() });
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
