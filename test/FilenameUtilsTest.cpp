#include "pch.h"
#include <kf/FilenameUtils.h>

SCENARIO("FilenameUtils getPathNoEndSeparator")
{
    GIVEN("various file paths with backslashes")
    {
        WHEN("getting path without end separator for native NT path")
        {
            kf::USimpleString simplePath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\kernel32.dll");
            auto simplePathResult = kf::FilenameUtils::getPathNoEndSeparator(simplePath);

            THEN("native NT path is extracted correctly without trailing separator")
            {
                REQUIRE(simplePathResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\Windows\\System32")));
            }
        }

        WHEN("getting path without end separator for MUP network path")
        {
            kf::USimpleString networkPath(L"\\Device\\Mup\\server\\share\\folder\\file.txt");
            auto networkPathResult = kf::FilenameUtils::getPathNoEndSeparator(networkPath);

            THEN("MUP network path is extracted correctly without trailing separator")
            {
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"\\Device\\Mup\\server\\share\\folder")));
            }
        }

        WHEN("getting path without end separator for path ending with backslash")
        {
            kf::USimpleString pathEndingWithBackslash(L"\\Device\\HarddiskVolume1\\Windows\\");
            auto pathEndingWithBackslashResult = kf::FilenameUtils::getPathNoEndSeparator(pathEndingWithBackslash);

            THEN("path ending with backslash is extracted correctly without trailing separator")
            {
                REQUIRE(pathEndingWithBackslashResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\Windows")));
            }
        }

        WHEN("getting path without end separator for root path")
        {
            kf::USimpleString rootPath(L"\\file.txt");
            auto rootPathResult = kf::FilenameUtils::getPathNoEndSeparator(rootPath);

            THEN("root path returns empty")
            {
                REQUIRE(rootPathResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getPathWithEndSeparator")
{
    GIVEN("various file paths with backslashes")
    {
        WHEN("getting path with end separator for native NT path")
        {
            kf::USimpleString simplePath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\kernel32.dll");
            auto simplePathResult = kf::FilenameUtils::getPathWithEndSeparator(simplePath);

            THEN("native NT path is extracted correctly with trailing separator")
            {
                REQUIRE(simplePathResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\Windows\\System32\\")));
            }
        }

        WHEN("getting path with end separator for MUP network path")
        {
            kf::USimpleString networkPath(L"\\Device\\Mup\\server\\share\\folder\\file.txt");
            auto networkPathResult = kf::FilenameUtils::getPathWithEndSeparator(networkPath);

            THEN("MUP network path is extracted correctly with trailing separator")
            {
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"\\Device\\Mup\\server\\share\\folder\\")));
            }
        }

        WHEN("getting path with end separator for root path")
        {
            kf::USimpleString rootPath(L"\\file.txt");
            auto rootPathResult = kf::FilenameUtils::getPathWithEndSeparator(rootPath);

            THEN("root path returns with trailing separator")
            {
                REQUIRE(rootPathResult.equals(kf::USimpleString(L"\\")));
            }
        }
    }
}

SCENARIO("FilenameUtils getFileNameNoStream")
{
    GIVEN("various file paths with and without streams")
    {
        WHEN("getting filename without stream for file with single stream")
        {
            kf::USimpleString fileWithStream(L"\\Device\\HarddiskVolume1\\folder\\file.txt:stream1");
            auto fileWithStreamResult = kf::FilenameUtils::getFileNameNoStream(fileWithStream);

            THEN("filename is extracted correctly without stream")
            {
                REQUIRE(fileWithStreamResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.txt")));
            }
        }

        WHEN("getting filename without stream for file with multiple streams")
        {
            kf::USimpleString fileWithMultipleStreams(L"\\Device\\HarddiskVolume1\\folder\\file.txt:stream1:stream2");
            auto fileWithMultipleStreamsResult = kf::FilenameUtils::getFileNameNoStream(fileWithMultipleStreams);

            THEN("filename is extracted correctly without any streams")
            {
                REQUIRE(fileWithMultipleStreamsResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.txt")));
            }
        }

        WHEN("getting filename without stream for file without stream")
        {
            kf::USimpleString fileWithoutStream(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto fileWithoutStreamResult = kf::FilenameUtils::getFileNameNoStream(fileWithoutStream);

            THEN("filename remains unchanged")
            {
                REQUIRE(fileWithoutStreamResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.txt")));
            }
        }

        WHEN("getting filename without stream for path with stream in folder name")
        {
            kf::USimpleString pathWithStreamInFolder(L"\\Device\\HarddiskVolume1\\folder:stream\\file.txt");
            auto pathWithStreamInFolderResult = kf::FilenameUtils::getFileNameNoStream(pathWithStreamInFolder);

            THEN("path remains unchanged as stream is in folder name")
            {
                REQUIRE(pathWithStreamInFolderResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder:stream\\file.txt")));
            }
        }

        WHEN("getting filename without stream for empty path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::getFileNameNoStream(emptyPath);

            THEN("empty path returns empty")
            {
                REQUIRE(emptyPathResult.equals(kf::USimpleString(L"")));
            }
        }
    }
}

SCENARIO("FilenameUtils getExtension")
{
    GIVEN("various file paths with and without extensions")
    {
        WHEN("getting extension from native NT file with single extension")
        {
            kf::USimpleString fileWithExt(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto fileWithExtResult = kf::FilenameUtils::getExtension(fileWithExt);

            THEN("extension is extracted correctly")
            {
                REQUIRE(fileWithExtResult.equals(kf::USimpleString(L"txt")));
            }
        }

        WHEN("getting extension from native NT file with multiple extensions")
        {
            kf::USimpleString fileWithMultipleExt(L"\\Device\\HarddiskVolume1\\folder\\file.tar.gz");
            auto fileWithMultipleExtResult = kf::FilenameUtils::getExtension(fileWithMultipleExt);

            THEN("last extension is extracted correctly")
            {
                REQUIRE(fileWithMultipleExtResult.equals(kf::USimpleString(L"gz")));
            }
        }

        WHEN("getting extension from native NT file without extension")
        {
            kf::USimpleString fileWithoutExt(L"\\Device\\HarddiskVolume1\\folder\\file");
            auto fileWithoutExtResult = kf::FilenameUtils::getExtension(fileWithoutExt);

            THEN("empty extension is returned")
            {
                REQUIRE(fileWithoutExtResult.isEmpty());
            }
        }

        WHEN("getting extension from native NT file with extension and stream")
        {
            kf::USimpleString fileWithExtAndStream(L"\\Device\\HarddiskVolume1\\folder\\file.txt:stream");
            auto fileWithExtAndStreamResult = kf::FilenameUtils::getExtension(fileWithExtAndStream);

            THEN("extension is extracted correctly ignoring stream")
            {
                REQUIRE(fileWithExtAndStreamResult.equals(kf::USimpleString(L"txt")));
            }
        }

        WHEN("getting extension from native NT path only")
        {
            kf::USimpleString pathOnly(L"\\Device\\HarddiskVolume1\\folder\\");
            auto pathOnlyResult = kf::FilenameUtils::getExtension(pathOnly);

            THEN("empty extension is returned")
            {
                REQUIRE(pathOnlyResult.isEmpty());
            }
        }

        WHEN("getting extension from empty path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::getExtension(emptyPath);

            THEN("empty extension is returned")
            {
                REQUIRE(emptyPathResult.isEmpty());
            }
        }

        WHEN("getting extension from native NT dot file")
        {
            kf::USimpleString dotFile(L"\\Device\\HarddiskVolume1\\folder\\.hidden");
            auto dotFileResult = kf::FilenameUtils::getExtension(dotFile);

            THEN("extension is extracted correctly")
            {
                REQUIRE(dotFileResult.equals(kf::USimpleString(L"hidden")));
            }
        }
    }
}

SCENARIO("FilenameUtils removeExtension")
{
    GIVEN("various file paths with and without extensions")
    {
        WHEN("removing extension from native NT file with extension")
        {
            kf::USimpleString fileWithExt(L"\\Device\\HarddiskVolume1\\folder\\file.txt");
            auto fileWithExtResult = kf::FilenameUtils::removeExtension(fileWithExt);

            THEN("extension is removed correctly")
            {
                REQUIRE(fileWithExtResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file")));
            }
        }

        WHEN("removing extension from native NT file with multiple extensions")
        {
            kf::USimpleString fileWithMultipleExt(L"\\Device\\HarddiskVolume1\\folder\\file.tar.gz");
            auto fileWithMultipleExtResult = kf::FilenameUtils::removeExtension(fileWithMultipleExt);

            THEN("last extension is removed correctly")
            {
                REQUIRE(fileWithMultipleExtResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file.tar")));
            }
        }

        WHEN("removing extension from native NT file without extension")
        {
            kf::USimpleString fileWithoutExt(L"\\Device\\HarddiskVolume1\\folder\\file");
            auto fileWithoutExtResult = kf::FilenameUtils::removeExtension(fileWithoutExt);

            THEN("file path remains unchanged")
            {
                REQUIRE(fileWithoutExtResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\file")));
            }
        }

        WHEN("removing extension from native NT path only")
        {
            kf::USimpleString pathOnly(L"\\Device\\HarddiskVolume1\\folder\\");
            auto pathOnlyResult = kf::FilenameUtils::removeExtension(pathOnly);

            THEN("path remains unchanged")
            {
                REQUIRE(pathOnlyResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\")));
            }
        }

        WHEN("removing extension from empty path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::removeExtension(emptyPath);

            THEN("empty path remains empty")
            {
                REQUIRE(emptyPathResult.isEmpty());
            }
        }

        WHEN("removing extension from native NT dot file")
        {
            kf::USimpleString dotFile(L"\\Device\\HarddiskVolume1\\folder\\.hidden");
            auto dotFileResult = kf::FilenameUtils::removeExtension(dotFile);

            THEN("extension is removed correctly")
            {
                REQUIRE(dotFileResult.equals(kf::USimpleString(L"\\Device\\HarddiskVolume1\\folder\\.")));
            }
        }
    }
}

SCENARIO("FilenameUtils getName")
{
    GIVEN("various file paths")
    {
        WHEN("getting filename from native NT full path")
        {
            kf::USimpleString fullPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\kernel32.dll");
            auto fullPathResult = kf::FilenameUtils::getName(fullPath);

            THEN("filename is extracted correctly")
            {
                REQUIRE(fullPathResult.equals(kf::USimpleString(L"kernel32.dll")));
            }
        }

        WHEN("getting filename from MUP network path")
        {
            kf::USimpleString networkPath(L"\\Device\\Mup\\server\\share\\folder\\file.txt");
            auto networkPathResult = kf::FilenameUtils::getName(networkPath);

            THEN("filename is extracted correctly")
            {
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"file.txt")));
            }
        }

        WHEN("getting filename from root file")
        {
            kf::USimpleString rootFile(L"\\file.txt");
            auto rootFileResult = kf::FilenameUtils::getName(rootFile);

            THEN("filename is extracted correctly")
            {
                REQUIRE(rootFileResult.equals(kf::USimpleString(L"file.txt")));
            }
        }

        WHEN("getting filename from path without backslash")
        {
            kf::USimpleString pathWithoutBackslash(L"file.txt");
            auto pathWithoutBackslashResult = kf::FilenameUtils::getName(pathWithoutBackslash);

            THEN("entire string is returned as filename")
            {
                REQUIRE(pathWithoutBackslashResult.equals(kf::USimpleString(L"file.txt")));
            }
        }

        WHEN("getting filename from native NT path ending with backslash")
        {
            kf::USimpleString pathEndingWithBackslash(L"\\Device\\HarddiskVolume1\\Windows\\");
            auto pathEndingWithBackslashResult = kf::FilenameUtils::getName(pathEndingWithBackslash);

            THEN("empty filename is returned")
            {
                REQUIRE(pathEndingWithBackslashResult.isEmpty());
            }
        }

        WHEN("getting filename from empty path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::getName(emptyPath);

            THEN("empty filename is returned")
            {
                REQUIRE(emptyPathResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getServerAndShareName")
{
    GIVEN("various network and local paths")
    {
        WHEN("getting server and share name from MUP path")
        {
            kf::USimpleString mupPath(L"\\device\\mup\\172.24.79.245\\my-dfs\\dir\\file");
            auto mupPathResult = kf::FilenameUtils::getServerAndShareName(mupPath);

            THEN("server and share name is extracted correctly")
            {
                REQUIRE(mupPathResult.equals(kf::USimpleString(L"\\172.24.79.245\\my-dfs")));
            }
        }

        WHEN("getting server and share name from MUP path with uppercase")
        {
            kf::USimpleString mupPathUpperCase(L"\\DEVICE\\MUP\\192.168.1.1\\share\\folder\\test.txt");
            auto mupPathUpperCaseResult = kf::FilenameUtils::getServerAndShareName(mupPathUpperCase);

            THEN("server and share name is extracted correctly ignoring case")
            {
                REQUIRE(mupPathUpperCaseResult.equals(kf::USimpleString(L"\\192.168.1.1\\share")));
            }
        }

        WHEN("getting server and share name from regular native NT path")
        {
            kf::USimpleString regularPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto regularPathResult = kf::FilenameUtils::getServerAndShareName(regularPath);

            THEN("empty result is returned for non-MUP path")
            {
                REQUIRE(regularPathResult.isEmpty());
            }
        }

        WHEN("getting server and share name from partial MUP path")
        {
            kf::USimpleString partialMupPath(L"\\device\\mup\\server");
            auto partialMupPathResult = kf::FilenameUtils::getServerAndShareName(partialMupPath);

            THEN("empty result is returned for incomplete MUP path")
            {
                REQUIRE(partialMupPathResult.isEmpty());
            }
        }

        WHEN("getting server and share name from empty path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::getServerAndShareName(emptyPath);

            THEN("empty result is returned for empty path")
            {
                REQUIRE(emptyPathResult.isEmpty());
            }
        }

        WHEN("getting server and share name from invalid device path")
        {
            kf::USimpleString invalidMupPath(L"\\device\\other\\server\\share");
            auto invalidMupPathResult = kf::FilenameUtils::getServerAndShareName(invalidMupPath);

            THEN("empty result is returned for non-MUP device path")
            {
                REQUIRE(invalidMupPathResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getNameCount")
{
    GIVEN("various paths with different element counts")
    {
        WHEN("counting elements in empty and minimal paths")
        {
            kf::USimpleString emptyPath(L"");
            kf::USimpleString rootPath(L"\\");
            kf::USimpleString doubleBackslash(L"\\\\");
            
            auto emptyPathCount = kf::FilenameUtils::getNameCount(emptyPath);
            auto rootPathCount = kf::FilenameUtils::getNameCount(rootPath);
            auto doubleBackslashCount = kf::FilenameUtils::getNameCount(doubleBackslash);

            THEN("minimal paths have zero elements")
            {
                REQUIRE(emptyPathCount == 0);
                REQUIRE(rootPathCount == 0);
                REQUIRE(doubleBackslashCount == 0);
            }
        }

        WHEN("counting elements in single element paths")
        {
            kf::USimpleString singleElement(L"aa");
            kf::USimpleString singleElementWithLeadingSlash(L"\\aa");
            kf::USimpleString singleElementWithTrailingSlash(L"\\aa\\");
            
            auto singleElementCount = kf::FilenameUtils::getNameCount(singleElement);
            auto singleElementWithLeadingSlashCount = kf::FilenameUtils::getNameCount(singleElementWithLeadingSlash);
            auto singleElementWithTrailingSlashCount = kf::FilenameUtils::getNameCount(singleElementWithTrailingSlash);

            THEN("single element paths have one element")
            {
                REQUIRE(singleElementCount == 1);
                REQUIRE(singleElementWithLeadingSlashCount == 1);
                REQUIRE(singleElementWithTrailingSlashCount == 1);
            }
        }

        WHEN("counting elements in two element paths")
        {
            kf::USimpleString twoElements(L"\\aa\\bb");
            kf::USimpleString twoElementsWithTrailingSlash(L"\\aa\\bb\\");
            kf::USimpleString twoElementsNoLeadingSlash(L"aa\\bb");
            
            auto twoElementsCount = kf::FilenameUtils::getNameCount(twoElements);
            auto twoElementsWithTrailingSlashCount = kf::FilenameUtils::getNameCount(twoElementsWithTrailingSlash);
            auto twoElementsNoLeadingSlashCount = kf::FilenameUtils::getNameCount(twoElementsNoLeadingSlash);

            THEN("two element paths have two elements")
            {
                REQUIRE(twoElementsCount == 2);
                REQUIRE(twoElementsWithTrailingSlashCount == 2);
                REQUIRE(twoElementsNoLeadingSlashCount == 2);
            }
        }
    }
}

SCENARIO("FilenameUtils subpath")
{
    GIVEN("paths with multiple elements")
    {
        WHEN("extracting single element subpaths")
        {
            kf::USimpleString pathAaBb(L"aa\\bb");
            kf::USimpleString pathWithLeadingSlash(L"\\aa\\bb");
            
            auto pathAaBbResult1 = kf::FilenameUtils::subpath(pathAaBb, 0, 1);
            auto pathWithLeadingSlashResult1 = kf::FilenameUtils::subpath(pathWithLeadingSlash, 0, 1);

            THEN("single elements are extracted correctly")
            {
                REQUIRE(pathAaBbResult1.equals(kf::USimpleString(L"aa")));
                REQUIRE(pathWithLeadingSlashResult1.equals(kf::USimpleString(L"aa")));
            }
        }

        WHEN("extracting multiple element subpaths")
        {
            kf::USimpleString pathWithLeadingSlash(L"\\aa\\bb");
            kf::USimpleString pathWithTrailingSlash(L"\\aa\\bb\\");
            kf::USimpleString longPath(L"\\one\\two\\three\\four\\five");
            
            auto pathWithLeadingSlashResult2 = kf::FilenameUtils::subpath(pathWithLeadingSlash, 0, 2);
            auto pathWithTrailingSlashResult = kf::FilenameUtils::subpath(pathWithTrailingSlash, 0, 2);
            auto longPathResult1 = kf::FilenameUtils::subpath(longPath, 1, 2);
            auto longPathResult2 = kf::FilenameUtils::subpath(longPath, 2);

            THEN("multiple elements are extracted correctly")
            {
                REQUIRE(pathWithLeadingSlashResult2.equals(kf::USimpleString(L"aa\\bb")));
                REQUIRE(pathWithTrailingSlashResult.equals(kf::USimpleString(L"aa\\bb")));
                REQUIRE(longPathResult1.equals(kf::USimpleString(L"two\\three")));
                REQUIRE(longPathResult2.equals(kf::USimpleString(L"three\\four\\five")));
            }
        }

        WHEN("extracting subpaths with invalid parameters")
        {
            kf::USimpleString pathAaBb(L"aa\\bb");
            kf::USimpleString emptyPath(L"");
            kf::USimpleString singleElement(L"test");
            
            auto emptyPathResult = kf::FilenameUtils::subpath(emptyPath, 0, 1);
            auto invalidIndexResult = kf::FilenameUtils::subpath(pathAaBb, -1, 1);
            auto invalidCountResult = kf::FilenameUtils::subpath(pathAaBb, 0, -1);
            auto outOfRangeResult = kf::FilenameUtils::subpath(singleElement, 5, 1);

            THEN("invalid parameters return empty results")
            {
                REQUIRE(emptyPathResult.isEmpty());
                REQUIRE(invalidIndexResult.isEmpty());
                REQUIRE(invalidCountResult.isEmpty());
                REQUIRE(outOfRangeResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils dosNameToNative")
{
    GIVEN("various DOS path formats")
    {
        WHEN("converting extended DOS path to native format")
        {
            kf::USimpleString extendedPath(L"\\\\?\\C:\\Windows\\System32\\file.txt");
            auto extendedPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(extendedPath);

            THEN("extended path is converted to native NT format")
            {
                REQUIRE(extendedPathResult.equals(kf::USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("converting UNC DOS path to native format")
        {
            kf::USimpleString uncPath(L"\\\\server\\share\\folder\\file.txt");
            auto uncPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(uncPath);

            THEN("UNC path is converted to MUP device format")
            {
                REQUIRE(uncPathResult.equals(kf::USimpleString(L"\\device\\mup\\server\\share\\folder\\file.txt")));
            }
        }

        WHEN("converting regular DOS path to native format")
        {
            kf::USimpleString regularDosPath(L"C:\\Windows\\System32\\file.txt");
            auto regularDosPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(regularDosPath);

            THEN("regular DOS path is converted to native NT format")
            {
                REQUIRE(regularDosPathResult.equals(kf::USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
            }
        }

        WHEN("converting empty DOS path to native format")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(emptyPath);

            THEN("empty path gets NT prefix")
            {
                REQUIRE(emptyPathResult.equals(kf::USimpleString(L"\\??\\")));
            }
        }
    }
}

SCENARIO("FilenameUtils isAbsoluteRegistryPath")
{
    GIVEN("various path types")
    {
        WHEN("checking if uppercase registry path is absolute")
        {
            kf::USimpleString registryPath(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Test");
            auto registryPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryPath);

            THEN("uppercase registry path is identified correctly")
            {
                REQUIRE(registryPathResult == true);
            }
        }

        WHEN("checking if lowercase registry path is absolute")
        {
            kf::USimpleString registryPathLowerCase(L"\\registry\\user\\test");
            auto registryPathLowerCaseResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryPathLowerCase);

            THEN("lowercase registry path is identified correctly")
            {
                REQUIRE(registryPathLowerCaseResult == true);
            }
        }

        WHEN("checking if regular native NT path is absolute registry path")
        {
            kf::USimpleString regularPath(L"\\Device\\HarddiskVolume1\\Windows\\System32\\file.txt");
            auto regularPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(regularPath);

            THEN("regular native NT path is not identified as registry path")
            {
                REQUIRE(regularPathResult == false);
            }
        }

        WHEN("checking if partial registry path is absolute")
        {
            kf::USimpleString partialRegistryPath(L"REGISTRY\\MACHINE\\test");
            auto partialRegistryPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(partialRegistryPath);

            THEN("partial registry path is not identified as absolute")
            {
                REQUIRE(partialRegistryPathResult == false);
            }
        }

        WHEN("checking if empty path is absolute registry path")
        {
            kf::USimpleString emptyPath(L"");
            auto emptyPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(emptyPath);

            THEN("empty path is not identified as registry path")
            {
                REQUIRE(emptyPathResult == false);
            }
        }

        WHEN("checking if native NT path containing registry keyword is absolute registry path")
        {
            kf::USimpleString registryInPath(L"\\Device\\HarddiskVolume1\\REGISTRY\\file.txt");
            auto registryInPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryInPath);

            THEN("path containing registry keyword is not identified as registry path")
            {
                REQUIRE(registryInPathResult == false);
            }
        }
    }
}