#include "pch.h"
#include <kf/FilenameUtils.h>

using namespace kf;

SCENARIO("FilenameUtils getPathNoEndSeparators")
{
    GIVEN("various file paths")
    {
        WHEN("getting path from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getPathNoEndSeparators(emptyPath);

            THEN("result is empty")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting path from path with several trailing slashes")
        {
            USimpleString pathWithTrailingSlashes(L"\\Device\\HarddiskVolume1\\Windows\\\\\\");
            auto result = FilenameUtils::getPathNoEndSeparators(pathWithTrailingSlashes);

            THEN("trailing separators are removed")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\Windows")));
            }
        }

        WHEN("getting path from path that consists only of slashes")
        {
            USimpleString slashesOnly(L"\\\\\\");
            auto result = FilenameUtils::getPathNoEndSeparators(slashesOnly);

            THEN("all trailing slashes are removed")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting path from path without any slashes")
        {
            USimpleString noSlashes(L"filename.txt");
            auto result = FilenameUtils::getPathNoEndSeparators(noSlashes);

            THEN("result is the same as input")
            {
                REQUIRE(result.equals(USimpleString(L"filename.txt")));
            }
        }

        WHEN("getting path from normal NT path without trailing slash")
        {
            USimpleString normalPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::getPathNoEndSeparators(normalPath);

            THEN("path remains unchanged since no trailing slash")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("getting path from relative path")
        {
            USimpleString relativePath(L"folder\\subfolder\\file.txt");
            auto result = FilenameUtils::getPathNoEndSeparators(relativePath);

            THEN("relative path remains unchanged since no trailing slash")
            {
                REQUIRE(result.equals(USimpleString(L"folder\\subfolder\\file.txt")));
            }
        }
    }
}

SCENARIO("FilenameUtils getPathWithEndSeparator")
{
    GIVEN("various file paths")
    {
        WHEN("getting path from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getPathWithEndSeparator(emptyPath);

            THEN("result remains empty")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting path from normal NT path")
        {
            USimpleString normalPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::getPathWithEndSeparator(normalPath);

            THEN("result remains unchanged since separator cannot be added")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("getting path from path without any slashes")
        {
            USimpleString noSlashes(L"filename.txt");
            auto result = FilenameUtils::getPathWithEndSeparator(noSlashes);

            THEN("result remains unchanged since no slashes exist")
            {
                REQUIRE(result.equals(USimpleString(L"filename.txt")));
            }
        }
    }
}

SCENARIO("FilenameUtils getFileNameNoStream")
{
    GIVEN("various file paths")
    {
        WHEN("getting filename from file with stream")
        {
            USimpleString fileWithStream(L"\\Device\\HarddiskVolume1\\folder\\file.txt:stream1");
            auto result = FilenameUtils::getFileNameNoStream(fileWithStream);

            THEN("filename is extracted without stream")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.txt")));
            }
        }

        WHEN("getting filename from file without stream")
        {
            USimpleString fileWithoutStream(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto result = FilenameUtils::getFileNameNoStream(fileWithoutStream);

            THEN("filename remains unchanged")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.txt")));
            }
        }

        WHEN("getting filename from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getFileNameNoStream(emptyPath);

            THEN("empty path returns empty")
            {
                REQUIRE(result.equals(USimpleString(L"")));
            }
        }
    }
}

SCENARIO("FilenameUtils getExtension")
{
    GIVEN("various file paths")
    {
        WHEN("getting extension from file with extension")
        {
            USimpleString fileWithExt(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto result = FilenameUtils::getExtension(fileWithExt);

            THEN("extension is extracted correctly")
            {
                REQUIRE(result.equals(USimpleString(L"txt")));
            }
        }

        WHEN("getting extension from file without extension")
        {
            USimpleString fileWithoutExt(L"\\Device\\HarddiskVolume1\\folder\\file");
            auto result = FilenameUtils::getExtension(fileWithoutExt);

            THEN("empty extension is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting extension from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getExtension(emptyPath);

            THEN("empty extension is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting extension from path only")
        {
            USimpleString pathOnly(L"\\Device\\HarddiskVolume1\\folder\\");
            auto result = FilenameUtils::getExtension(pathOnly);

            THEN("empty extension is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils removeExtension")
{
    GIVEN("various file paths")
    {
        WHEN("removing extension from file with extension")
        {
            USimpleString fileWithExt(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto result = FilenameUtils::removeExtension(fileWithExt);

            THEN("extension is removed correctly")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file")));
            }
        }

        WHEN("removing extension from file without extension")
        {
            USimpleString fileWithoutExt(L"\\Device\\HarddiskVolume1\\folder\\file");
            auto result = FilenameUtils::removeExtension(fileWithoutExt);

            THEN("file path remains unchanged")
            {
                REQUIRE(result.equals(USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file")));
            }
        }

        WHEN("removing extension from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::removeExtension(emptyPath);

            THEN("empty path remains empty")
            {
                REQUIRE(result.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getName")
{
    GIVEN("various file paths")
    {
        WHEN("getting filename from full path")
        {
            USimpleString fullPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\kernel32.dll");
            auto result = FilenameUtils::getName(fullPath);

            THEN("filename is extracted correctly")
            {
                REQUIRE(result.equals(USimpleString(L"kernel32.dll")));
            }
        }

        WHEN("getting filename from path without slashes")
        {
            USimpleString pathWithoutSlashes(L"file.txt");
            auto result = FilenameUtils::getName(pathWithoutSlashes);

            THEN("entire string is returned as filename")
            {
                REQUIRE(result.equals(USimpleString(L"file.txt")));
            }
        }

        WHEN("getting filename from path ending with slash")
        {
            USimpleString pathEndingWithSlash(L"\\Device\\HarddiskVolume1\\Windows\\");
            auto result = FilenameUtils::getName(pathEndingWithSlash);

            THEN("empty filename is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting filename from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getName(emptyPath);

            THEN("empty filename is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getServerAndShareName")
{
    GIVEN("various network and local paths")
    {
        WHEN("getting server and share from MUP path")
        {
            USimpleString mupPath(L"\\device\\mup\\172.24.79.245\\my-dfs\\dir\\file");
            auto result = FilenameUtils::getServerAndShareName(mupPath);

            THEN("server and share name is extracted correctly")
            {
                REQUIRE(result.equals(USimpleString(L"\\172.24.79.245\\my-dfs")));
            }
        }

        WHEN("getting server and share from regular path")
        {
            USimpleString regularPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::getServerAndShareName(regularPath);

            THEN("empty result is returned for non-MUP path")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting server and share from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getServerAndShareName(emptyPath);

            THEN("empty result is returned for empty path")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting server and share from MUP path with only server (no share)")
        {
            USimpleString mupPathNoShare(L"\\device\\mup\\server.domain.com");
            auto result = FilenameUtils::getServerAndShareName(mupPathNoShare);

            THEN("empty result is returned since function requires both server and share")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("getting server and share from MUP path with FQDN server name")
        {
            USimpleString mupPathFQDN(L"\\device\\mup\\server.domain.com\\share\\dir\\file");
            auto result = FilenameUtils::getServerAndShareName(mupPathFQDN);

            THEN("FQDN server and share name is extracted correctly")
            {
                REQUIRE(result.equals(USimpleString(L"\\server.domain.com\\share")));
            }
        }

        WHEN("getting server and share from UPPERCASED MUP path")
        {
            USimpleString upperMupPath(L"\\DEVICE\\MUP\\172.24.79.245\\MY-DFS\\DIR\\FILE");
            auto result = FilenameUtils::getServerAndShareName(upperMupPath);

            THEN("server and share name is extracted correctly despite case")
            {
                REQUIRE(result.equals(USimpleString(L"\\172.24.79.245\\MY-DFS")));
            }
        }
    }
}

SCENARIO("FilenameUtils getNameCount")
{
    GIVEN("various paths")
    {
        WHEN("counting elements in empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::getNameCount(emptyPath);

            THEN("element count is zero")
            {
                REQUIRE(result == 0);
            }
        }

        WHEN("counting elements in path with only slashes")
        {
            USimpleString slashesOnly(L"\\\\");
            auto result = FilenameUtils::getNameCount(slashesOnly);

            THEN("element count is zero")
            {
                REQUIRE(result == 0);
            }
        }

        WHEN("counting elements in single element path")
        {
            USimpleString singleElement(L"\\aa");
            auto result = FilenameUtils::getNameCount(singleElement);

            THEN("element count is one")
            {
                REQUIRE(result == 1);
            }
        }

        WHEN("counting elements in multi-element path")
        {
            USimpleString multiElement(L"\\aa\\bb\\cc");
            auto result = FilenameUtils::getNameCount(multiElement);

            THEN("element count is correct")
            {
                REQUIRE(result == 3);
            }
        }

        WHEN("counting elements in path with slash at the end")
        {
            USimpleString pathWithEndSlash(L"\\aa\\bb\\cc\\");
            auto result = FilenameUtils::getNameCount(pathWithEndSlash);

            THEN("element count ignores trailing slash")
            {
                REQUIRE(result == 3);
            }
        }
    }
}

SCENARIO("FilenameUtils subpath")
{
    GIVEN("paths with multiple elements")
    {
        WHEN("extracting subpath from valid path")
        {
            USimpleString path(L"\\aa\\bb\\cc");
            auto result = FilenameUtils::subpath(path, 0, 2);

            THEN("subpath is extracted correctly")
            {
                REQUIRE(result.equals(USimpleString(L"aa\\bb")));
            }
        }

        WHEN("extracting subpath with invalid parameters")
        {
            USimpleString path(L"\\aa\\bb");
            auto result = FilenameUtils::subpath(path, -1, 1);

            THEN("empty result is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("extracting subpath from empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::subpath(emptyPath, 0, 1);

            THEN("empty result is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("extracting subpath where both parameters are incorrect")
        {
            USimpleString path(L"\\aa\\bb");
            auto result = FilenameUtils::subpath(path, -1, -1);

            THEN("empty result is returned")
            {
                REQUIRE(result.isEmpty());
            }
        }

        WHEN("extracting subpath where both parameters are equal")
        {
            USimpleString path(L"\\aa\\bb\\cc");
            auto result = FilenameUtils::subpath(path, 1, 1);

            THEN("single element is extracted")
            {
                REQUIRE(result.equals(USimpleString(L"bb")));
            }
        }
    }
}

SCENARIO("FilenameUtils dosNameToNative")
{
    GIVEN("various DOS path formats")
    {
        WHEN("converting extended DOS path")
        {
            USimpleString extendedPath(L"\\\\?\\C:\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::dosNameToNative<PagedPool>(extendedPath);

            THEN("path is converted to native NT format")
            {
                REQUIRE(result.equals(USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("converting UNC DOS path")
        {
            USimpleString uncPath(L"\\\\server\\share\\folder\\file.txt");
            auto result = FilenameUtils::dosNameToNative<PagedPool>(uncPath);

            THEN("path is converted to MUP device format")
            {
                REQUIRE(result.equals(USimpleString(L"\\device\\mup\\server\\share\\folder\\file.txt")));
            }
        }

        WHEN("converting regular DOS path")
        {
            USimpleString regularPath(L"C:\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::dosNameToNative<PagedPool>(regularPath);

            THEN("path is converted to native NT format")
            {
                REQUIRE(result.equals(USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("converting empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::dosNameToNative<PagedPool>(emptyPath);

            THEN("result should be empty")
            {
                REQUIRE(result.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils isAbsoluteRegistryPath")
{
    GIVEN("various path types")
    {
        WHEN("checking registry path")
        {
            USimpleString registryPath(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Test");
            auto result = FilenameUtils::isAbsoluteRegistryPath(registryPath);

            THEN("registry path is identified correctly")
            {
                REQUIRE(result == true);
            }
        }

        WHEN("checking regular path")
        {
            USimpleString regularPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto result = FilenameUtils::isAbsoluteRegistryPath(regularPath);

            THEN("regular path is not identified as registry path")
            {
                REQUIRE(result == false);
            }
        }

        WHEN("checking empty path")
        {
            USimpleString emptyPath(L"");
            auto result = FilenameUtils::isAbsoluteRegistryPath(emptyPath);

            THEN("empty path is not identified as registry path")
            {
                REQUIRE(result == false);
            }
        }
    }
}