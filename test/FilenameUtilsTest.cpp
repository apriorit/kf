#include "pch.h"
#include <kf/FilenameUtils.h>

SCENARIO("FilenameUtils getPathNoEndSeparator")
{
    GIVEN("various file paths")
    {
        kf::USimpleString emptyPath(L"");
        kf::USimpleString rootPath(L"\\");
        kf::USimpleString simplePath(L"C:\\Windows\\System32\\kernel32.dll");
        kf::USimpleString networkPath(L"\\\\server\\share\\folder\\file.txt");
        kf::USimpleString pathWithoutBackslash(L"file.txt");
        kf::USimpleString pathEndingWithBackslash(L"C:\\Windows\\");

        WHEN("getting path without end separator")
        {
            auto emptyPathResult = kf::FilenameUtils::getPathNoEndSeparator(emptyPath);
            auto rootPathResult = kf::FilenameUtils::getPathNoEndSeparator(rootPath);
            auto simplePathResult = kf::FilenameUtils::getPathNoEndSeparator(simplePath);
            auto networkPathResult = kf::FilenameUtils::getPathNoEndSeparator(networkPath);
            auto pathWithoutBackslashResult = kf::FilenameUtils::getPathNoEndSeparator(pathWithoutBackslash);
            auto pathEndingWithBackslashResult = kf::FilenameUtils::getPathNoEndSeparator(pathEndingWithBackslash);

            THEN("paths are extracted correctly without trailing separator")
            {
                REQUIRE(emptyPathResult.isEmpty());
                REQUIRE(rootPathResult.isEmpty());
                REQUIRE(simplePathResult.equals(kf::USimpleString(L"C:\\Windows\\System32")));
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"\\\\server\\share\\folder")));
                REQUIRE(pathWithoutBackslashResult.isEmpty());
                REQUIRE(pathEndingWithBackslashResult.equals(kf::USimpleString(L"C:\\Windows")));
            }
        }
    }
}

SCENARIO("FilenameUtils getPathWithEndSeparator")
{
    GIVEN("various file paths")
    {
        kf::USimpleString emptyPath(L"");
        kf::USimpleString rootPath(L"\\");
        kf::USimpleString simplePath(L"C:\\Windows\\System32\\kernel32.dll");
        kf::USimpleString networkPath(L"\\\\server\\share\\folder\\file.txt");
        kf::USimpleString pathWithoutBackslash(L"file.txt");

        WHEN("getting path with end separator")
        {
            auto emptyPathResult = kf::FilenameUtils::getPathWithEndSeparator(emptyPath);
            auto rootPathResult = kf::FilenameUtils::getPathWithEndSeparator(rootPath);
            auto simplePathResult = kf::FilenameUtils::getPathWithEndSeparator(simplePath);
            auto networkPathResult = kf::FilenameUtils::getPathWithEndSeparator(networkPath);
            auto pathWithoutBackslashResult = kf::FilenameUtils::getPathWithEndSeparator(pathWithoutBackslash);

            THEN("paths are extracted correctly with trailing separator")
            {
                REQUIRE(emptyPathResult.equals(kf::USimpleString(L"\\")));
                REQUIRE(rootPathResult.equals(kf::USimpleString(L"\\")));
                REQUIRE(simplePathResult.equals(kf::USimpleString(L"C:\\Windows\\System32\\")));
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"\\\\server\\share\\folder\\")));
                REQUIRE(pathWithoutBackslashResult.equals(kf::USimpleString(L"\\")));
            }
        }
    }
}

SCENARIO("FilenameUtils getFileNameNoStream")
{
    GIVEN("various file paths with and without streams")
    {
        kf::USimpleString fileWithStream(L"C:\\folder\\file.txt:stream1");
        kf::USimpleString fileWithMultipleStreams(L"C:\\folder\\file.txt:stream1:stream2");
        kf::USimpleString fileWithoutStream(L"C:\\folder\\file.txt");
        kf::USimpleString pathWithStreamInFolder(L"C:\\folder:stream\\file.txt");
        kf::USimpleString emptyPath(L"");

        WHEN("getting filename without stream")
        {
            auto fileWithStreamResult = kf::FilenameUtils::getFileNameNoStream(fileWithStream);
            auto fileWithMultipleStreamsResult = kf::FilenameUtils::getFileNameNoStream(fileWithMultipleStreams);
            auto fileWithoutStreamResult = kf::FilenameUtils::getFileNameNoStream(fileWithoutStream);
            auto pathWithStreamInFolderResult = kf::FilenameUtils::getFileNameNoStream(pathWithStreamInFolder);
            auto emptyPathResult = kf::FilenameUtils::getFileNameNoStream(emptyPath);

            THEN("filenames are extracted correctly without streams")
            {
                REQUIRE(fileWithStreamResult.equals(kf::USimpleString(L"C:\\folder\\file.txt")));
                REQUIRE(fileWithMultipleStreamsResult.equals(kf::USimpleString(L"C:\\folder\\file.txt")));
                REQUIRE(fileWithoutStreamResult.equals(kf::USimpleString(L"C:\\folder\\file.txt")));
                REQUIRE(pathWithStreamInFolderResult.equals(kf::USimpleString(L"C:\\folder:stream\\file.txt")));
                REQUIRE(emptyPathResult.equals(kf::USimpleString(L"")));
            }
        }
    }
}

SCENARIO("FilenameUtils getExtension")
{
    GIVEN("various file paths with and without extensions")
    {
        kf::USimpleString fileWithExt(L"C:\\folder\\file.txt");
        kf::USimpleString fileWithMultipleExt(L"C:\\folder\\file.tar.gz");
        kf::USimpleString fileWithoutExt(L"C:\\folder\\file");
        kf::USimpleString fileWithExtAndStream(L"C:\\folder\\file.txt:stream");
        kf::USimpleString pathOnly(L"C:\\folder\\");
        kf::USimpleString emptyPath(L"");
        kf::USimpleString dotFile(L"C:\\folder\\.hidden");

        WHEN("getting file extension")
        {
            auto fileWithExtResult = kf::FilenameUtils::getExtension(fileWithExt);
            auto fileWithMultipleExtResult = kf::FilenameUtils::getExtension(fileWithMultipleExt);
            auto fileWithoutExtResult = kf::FilenameUtils::getExtension(fileWithoutExt);
            auto fileWithExtAndStreamResult = kf::FilenameUtils::getExtension(fileWithExtAndStream);
            auto pathOnlyResult = kf::FilenameUtils::getExtension(pathOnly);
            auto emptyPathResult = kf::FilenameUtils::getExtension(emptyPath);
            auto dotFileResult = kf::FilenameUtils::getExtension(dotFile);

            THEN("extensions are extracted correctly")
            {
                REQUIRE(fileWithExtResult.equals(kf::USimpleString(L"txt")));
                REQUIRE(fileWithMultipleExtResult.equals(kf::USimpleString(L"gz")));
                REQUIRE(fileWithoutExtResult.isEmpty());
                REQUIRE(fileWithExtAndStreamResult.equals(kf::USimpleString(L"txt")));
                REQUIRE(pathOnlyResult.isEmpty());
                REQUIRE(emptyPathResult.isEmpty());
                REQUIRE(dotFileResult.equals(kf::USimpleString(L"hidden")));
            }
        }
    }
}

SCENARIO("FilenameUtils removeExtension")
{
    GIVEN("various file paths with and without extensions")
    {
        kf::USimpleString fileWithExt(L"C:\\folder\\file.txt");
        kf::USimpleString fileWithMultipleExt(L"C:\\folder\\file.tar.gz");
        kf::USimpleString fileWithoutExt(L"C:\\folder\\file");
        kf::USimpleString pathOnly(L"C:\\folder\\");
        kf::USimpleString emptyPath(L"");
        kf::USimpleString dotFile(L"C:\\folder\\.hidden");

        WHEN("removing file extension")
        {
            auto fileWithExtResult = kf::FilenameUtils::removeExtension(fileWithExt);
            auto fileWithMultipleExtResult = kf::FilenameUtils::removeExtension(fileWithMultipleExt);
            auto fileWithoutExtResult = kf::FilenameUtils::removeExtension(fileWithoutExt);
            auto pathOnlyResult = kf::FilenameUtils::removeExtension(pathOnly);
            auto emptyPathResult = kf::FilenameUtils::removeExtension(emptyPath);
            auto dotFileResult = kf::FilenameUtils::removeExtension(dotFile);

            THEN("extensions are removed correctly")
            {
                REQUIRE(fileWithExtResult.equals(kf::USimpleString(L"C:\\folder\\file")));
                REQUIRE(fileWithMultipleExtResult.equals(kf::USimpleString(L"C:\\folder\\file.tar")));
                REQUIRE(fileWithoutExtResult.equals(kf::USimpleString(L"C:\\folder\\file")));
                REQUIRE(pathOnlyResult.equals(kf::USimpleString(L"C:\\folder\\")));
                REQUIRE(emptyPathResult.isEmpty());
                REQUIRE(dotFileResult.equals(kf::USimpleString(L"C:\\folder\\.")));
            }
        }
    }
}

SCENARIO("FilenameUtils getName")
{
    GIVEN("various file paths")
    {
        kf::USimpleString fullPath(L"C:\\Windows\\System32\\kernel32.dll");
        kf::USimpleString networkPath(L"\\\\server\\share\\folder\\file.txt");
        kf::USimpleString rootFile(L"\\file.txt");
        kf::USimpleString pathWithoutBackslash(L"file.txt");
        kf::USimpleString pathEndingWithBackslash(L"C:\\Windows\\");
        kf::USimpleString emptyPath(L"");

        WHEN("getting filename from path")
        {
            auto fullPathResult = kf::FilenameUtils::getName(fullPath);
            auto networkPathResult = kf::FilenameUtils::getName(networkPath);
            auto rootFileResult = kf::FilenameUtils::getName(rootFile);
            auto pathWithoutBackslashResult = kf::FilenameUtils::getName(pathWithoutBackslash);
            auto pathEndingWithBackslashResult = kf::FilenameUtils::getName(pathEndingWithBackslash);
            auto emptyPathResult = kf::FilenameUtils::getName(emptyPath);

            THEN("filenames are extracted correctly")
            {
                REQUIRE(fullPathResult.equals(kf::USimpleString(L"kernel32.dll")));
                REQUIRE(networkPathResult.equals(kf::USimpleString(L"file.txt")));
                REQUIRE(rootFileResult.equals(kf::USimpleString(L"file.txt")));
                REQUIRE(pathWithoutBackslashResult.equals(kf::USimpleString(L"file.txt")));
                REQUIRE(pathEndingWithBackslashResult.isEmpty());
                REQUIRE(emptyPathResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getServerAndShareName")
{
    GIVEN("various network and local paths")
    {
        kf::USimpleString mupPath(L"\\device\\mup\\172.24.79.245\\my-dfs\\dir\\file");
        kf::USimpleString mupPathUpperCase(L"\\DEVICE\\MUP\\192.168.1.1\\share\\folder\\test.txt");
        kf::USimpleString regularPath(L"C:\\Windows\\System32\\file.txt");
        kf::USimpleString partialMupPath(L"\\device\\mup\\server");
        kf::USimpleString emptyPath(L"");
        kf::USimpleString invalidMupPath(L"\\device\\other\\server\\share");

        WHEN("getting server and share name")
        {
            auto mupPathResult = kf::FilenameUtils::getServerAndShareName(mupPath);
            auto mupPathUpperCaseResult = kf::FilenameUtils::getServerAndShareName(mupPathUpperCase);
            auto regularPathResult = kf::FilenameUtils::getServerAndShareName(regularPath);
            auto partialMupPathResult = kf::FilenameUtils::getServerAndShareName(partialMupPath);
            auto emptyPathResult = kf::FilenameUtils::getServerAndShareName(emptyPath);
            auto invalidMupPathResult = kf::FilenameUtils::getServerAndShareName(invalidMupPath);

            THEN("server and share names are extracted correctly")
            {
                REQUIRE(mupPathResult.equals(kf::USimpleString(L"\\172.24.79.245\\my-dfs")));
                REQUIRE(mupPathUpperCaseResult.equals(kf::USimpleString(L"\\192.168.1.1\\share")));
                REQUIRE(regularPathResult.isEmpty());
                REQUIRE(partialMupPathResult.isEmpty());
                REQUIRE(emptyPathResult.isEmpty());
                REQUIRE(invalidMupPathResult.isEmpty());
            }
        }
    }
}

SCENARIO("FilenameUtils getNameCount")
{
    GIVEN("various paths with different element counts")
    {
        kf::USimpleString emptyPath(L"");
        kf::USimpleString rootPath(L"\\");
        kf::USimpleString doubleBackslash(L"\\\\");
        kf::USimpleString singleElement(L"aa");
        kf::USimpleString singleElementWithLeadingSlash(L"\\aa");
        kf::USimpleString singleElementWithTrailingSlash(L"\\aa\\");
        kf::USimpleString twoElements(L"\\aa\\bb");
        kf::USimpleString twoElementsWithTrailingSlash(L"\\aa\\bb\\");
        kf::USimpleString twoElementsNoLeadingSlash(L"aa\\bb");

        WHEN("counting path elements")
        {
            auto emptyPathCount = kf::FilenameUtils::getNameCount(emptyPath);
            auto rootPathCount = kf::FilenameUtils::getNameCount(rootPath);
            auto doubleBackslashCount = kf::FilenameUtils::getNameCount(doubleBackslash);
            auto singleElementCount = kf::FilenameUtils::getNameCount(singleElement);
            auto singleElementWithLeadingSlashCount = kf::FilenameUtils::getNameCount(singleElementWithLeadingSlash);
            auto singleElementWithTrailingSlashCount = kf::FilenameUtils::getNameCount(singleElementWithTrailingSlash);
            auto twoElementsCount = kf::FilenameUtils::getNameCount(twoElements);
            auto twoElementsWithTrailingSlashCount = kf::FilenameUtils::getNameCount(twoElementsWithTrailingSlash);
            auto twoElementsNoLeadingSlashCount = kf::FilenameUtils::getNameCount(twoElementsNoLeadingSlash);

            THEN("element counts are correct")
            {
                REQUIRE(emptyPathCount == 0);
                REQUIRE(rootPathCount == 0);
                REQUIRE(doubleBackslashCount == 0);
                REQUIRE(singleElementCount == 1);
                REQUIRE(singleElementWithLeadingSlashCount == 1);
                REQUIRE(singleElementWithTrailingSlashCount == 1);
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
        kf::USimpleString pathAaBb(L"aa\\bb");
        kf::USimpleString pathWithLeadingSlash(L"\\aa\\bb");
        kf::USimpleString pathWithTrailingSlash(L"\\aa\\bb\\");
        kf::USimpleString longPath(L"\\one\\two\\three\\four\\five");
        kf::USimpleString emptyPath(L"");
        kf::USimpleString singleElement(L"test");

        WHEN("extracting subpaths with various parameters")
        {
            auto pathAaBbResult1 = kf::FilenameUtils::subpath(pathAaBb, 0, 1);
            auto pathWithLeadingSlashResult1 = kf::FilenameUtils::subpath(pathWithLeadingSlash, 0, 1);
            auto pathWithLeadingSlashResult2 = kf::FilenameUtils::subpath(pathWithLeadingSlash, 0, 2);
            auto pathWithTrailingSlashResult = kf::FilenameUtils::subpath(pathWithTrailingSlash, 0, 2);
            auto longPathResult1 = kf::FilenameUtils::subpath(longPath, 1, 2);
            auto longPathResult2 = kf::FilenameUtils::subpath(longPath, 2);
            auto emptyPathResult = kf::FilenameUtils::subpath(emptyPath, 0, 1);
            auto invalidIndexResult = kf::FilenameUtils::subpath(pathAaBb, -1, 1);
            auto invalidCountResult = kf::FilenameUtils::subpath(pathAaBb, 0, -1);
            auto outOfRangeResult = kf::FilenameUtils::subpath(singleElement, 5, 1);

            THEN("subpaths are extracted correctly")
            {
                REQUIRE(pathAaBbResult1.equals(kf::USimpleString(L"aa")));
                REQUIRE(pathWithLeadingSlashResult1.equals(kf::USimpleString(L"aa")));
                REQUIRE(pathWithLeadingSlashResult2.equals(kf::USimpleString(L"aa\\bb")));
                REQUIRE(pathWithTrailingSlashResult.equals(kf::USimpleString(L"aa\\bb")));
                REQUIRE(longPathResult1.equals(kf::USimpleString(L"two\\three")));
                REQUIRE(longPathResult2.equals(kf::USimpleString(L"three\\four\\five")));
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
        kf::USimpleString extendedPath(L"\\\\?\\C:\\Windows\\System32\\file.txt");
        kf::USimpleString uncPath(L"\\\\server\\share\\folder\\file.txt");
        kf::USimpleString regularDosPath(L"C:\\Windows\\System32\\file.txt");
        kf::USimpleString emptyPath(L"");

        WHEN("converting DOS names to native format")
        {
            auto extendedPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(extendedPath);
            auto uncPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(uncPath);
            auto regularDosPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(regularDosPath);
            auto emptyPathResult = kf::FilenameUtils::dosNameToNative<PagedPool>(emptyPath);

            THEN("paths are converted to native format correctly")
            {
                REQUIRE(extendedPathResult.equals(kf::USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
                REQUIRE(uncPathResult.equals(kf::USimpleString(L"\\device\\mup\\server\\share\\folder\\file.txt")));
                REQUIRE(regularDosPathResult.equals(kf::USimpleString(L"\\??\\C:\\Windows\\System32\\file.txt")));
                REQUIRE(emptyPathResult.equals(kf::USimpleString(L"\\??\\")));
            }
        }
    }
}

SCENARIO("FilenameUtils isAbsoluteRegistryPath")
{
    GIVEN("various path types")
    {
        kf::USimpleString registryPath(L"\\REGISTRY\\MACHINE\\SOFTWARE\\Test");
        kf::USimpleString registryPathLowerCase(L"\\registry\\user\\test");
        kf::USimpleString regularPath(L"C:\\Windows\\System32\\file.txt");
        kf::USimpleString partialRegistryPath(L"REGISTRY\\MACHINE\\test");
        kf::USimpleString emptyPath(L"");
        kf::USimpleString registryInPath(L"C:\\REGISTRY\\file.txt");

        WHEN("checking if paths are absolute registry paths")
        {
            auto registryPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryPath);
            auto registryPathLowerCaseResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryPathLowerCase);
            auto regularPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(regularPath);
            auto partialRegistryPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(partialRegistryPath);
            auto emptyPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(emptyPath);
            auto registryInPathResult = kf::FilenameUtils::isAbsoluteRegistryPath(registryInPath);

            THEN("registry paths are identified correctly")
            {
                REQUIRE(registryPathResult == true);
                REQUIRE(registryPathLowerCaseResult == true);
                REQUIRE(regularPathResult == false);
                REQUIRE(partialRegistryPathResult == false);
                REQUIRE(emptyPathResult == false);
                REQUIRE(registryInPathResult == false);
            }
        }
    }
}