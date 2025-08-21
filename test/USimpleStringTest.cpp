#include "pch.h"
#include <kf/USimpleString.h>

SCENARIO("USimpleString: all methods")
{
    GIVEN("Empty string")
    {
        kf::USimpleString str;

        WHEN("isEmpty() is called")
        {
            THEN("it returns true")
            {
                REQUIRE(str.isEmpty());
            }
        }

        WHEN("byteLength() is called")
        {
            THEN("it returns 0")
            {
                REQUIRE(str.byteLength() == 0);
            }
        }

        WHEN("maxByteLength() is called")
        {
            THEN("it returns 0")
            {
                REQUIRE(str.maxByteLength() == 0);
            }
        }

        WHEN("charLength() is called")
        {
            THEN("it returns 0")
            {
                REQUIRE(str.charLength() == 0);
            }
        }

        WHEN("maxCharLength() is called")
        {
            THEN("it returns 0")
            {
                REQUIRE(str.maxCharLength() == 0);
            }
        }

        WHEN("begin() is called")
        {
            THEN("it returns a null pointer")
            {
                REQUIRE(!str.begin());
            }
        }

        WHEN("end() is called")
        {
            THEN("it returns a null pointer")
            {
                REQUIRE(!str.end());
            }
        }

        WHEN("buffer() is called")
        {
            THEN("it returns a null pointer")
            {
                REQUIRE(!str.buffer());
            }
        }

        WHEN("compareTo() is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns 0")
            {
                REQUIRE(str.compareTo(emptyStr.string()) == 0);
            }
        }

        WHEN("compareTo() is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");
            THEN("it returns a negative value")
            {
                REQUIRE(str.compareTo(nonEmptyStr.string()) < 0);
            }
        }

        WHEN("compareToIgnoreCase is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns 0")
            {
                REQUIRE(str.compareToIgnoreCase(emptyStr.string()) == 0);
            }
        }
        
        WHEN("compareToIgnoreCase is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");

            THEN("it returns a negative value")
            {
                REQUIRE(str.compareToIgnoreCase(nonEmptyStr.string()) < 0);
            }
        }

        WHEN("equals() is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns true")
            {
                REQUIRE(str.equals(emptyStr.string()));
            }
        }

        WHEN("equals() is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.equals(nonEmptyStr.string()));
            }
        }

        WHEN("equalsIgnoreCase() is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns true")
            {
                REQUIRE(str.equalsIgnoreCase(emptyStr.string()));
            }
        }

        WHEN("equalsIgnoreCase() is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.equalsIgnoreCase(nonEmptyStr.string()));
            }
        }

        WHEN("operator< is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns false")
            {
                REQUIRE(!(str < emptyStr));
            }
        }

        WHEN("operator< is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");

            THEN("it returns true")
            {
                REQUIRE(str < nonEmptyStr);
            }
        }

        WHEN("operator== is called with empty string")
        {
            kf::USimpleString emptyStr;

            THEN("it returns true")
            {
                REQUIRE(str == emptyStr);
            }
        }

        WHEN("operator== is called with non-empty string")
        {
            kf::USimpleString nonEmptyStr(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!(str == nonEmptyStr));
            }
        }

        WHEN("split() is called")
        {
            int fromIndex = 0;
            kf::USimpleString result = str.split(L',', fromIndex);

            THEN("it returns an empty string")
            {
                REQUIRE(result.isEmpty());
                REQUIRE(fromIndex == -1);
            }
        }

        WHEN("contains() is called with empty substring")
        {
            kf::USimpleString substring;

            THEN("it returns true")
            {
                REQUIRE(str.contains(substring));
            }
        }

        WHEN("contains() is called with non-empty substring")
        {
            kf::USimpleString substring(L"Test");

            THEN("it returns false")
            {
                REQUIRE(!str.contains(substring));
            }
        }

        WHEN("indexOf() is called")
        {
            int index = str.indexOf(L'X');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("indexOf() is called with null character")
        {
            int index = str.indexOf(L'\0');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("lastIndexOf is called")
        {
            int index = str.lastIndexOf(L'Y');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("lastIndexOf is called with null character")
        {
            int index = str.lastIndexOf(L'\0');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("substring() is called")
        {
            kf::USimpleString subStr = str.substring(0, 0);

            THEN("it returns an empty string")
            {
                REQUIRE(subStr.isEmpty());
            }
        }

        // TODO: test substring with invalid parameters after removing ASSERTs

        WHEN("trim() is called")
        {
            kf::USimpleString trimmedStr = str.trim(L' ');

            THEN("it returns an empty string")
            {
                REQUIRE(trimmedStr.isEmpty());
            }
        }

        WHEN("trimRight() is called")
        {
            kf::USimpleString trimmedStr = str.trimRight(L' ');

            THEN("it returns an empty string")
            {
                REQUIRE(trimmedStr.isEmpty());
            }
        }

        WHEN("trimLeft() is called")
        {
            kf::USimpleString trimmedStr = str.trimLeft(L' ');

            THEN("it returns an empty string")
            {
                REQUIRE(trimmedStr.isEmpty());
            }
        }

        WHEN("startsWith() is called with empty substring")
        {
            kf::USimpleString prefix;

            THEN("it returns true")
            {
                REQUIRE(str.startsWith(prefix));
            }
        }

        WHEN("startsWith() is called with non-empty substring")
        {
            kf::USimpleString prefix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.startsWith(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with empty substring")
        {
            kf::USimpleString prefix;

            THEN("it returns true")
            {
                REQUIRE(str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with non-empty substring")
        {
            kf::USimpleString prefix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("endsWith() is called with empty substring")
        {
            kf::USimpleString suffix;

            THEN("it returns true")
            {
                REQUIRE(str.endsWith(suffix));
            }
        }

        WHEN("endsWith() is called with non-empty substring")
        {
            kf::USimpleString suffix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.endsWith(suffix));
            }
        }

        WHEN("endsWithIgnoreCase() is called with empty substring")
        {
            kf::USimpleString suffix;

            THEN("it returns true")
            {
                REQUIRE(str.endsWithIgnoreCase(suffix));
            }
        }

        WHEN("endsWithIgnoreCase() is called with non-empty substring")
        {
            kf::USimpleString suffix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.endsWithIgnoreCase(suffix));
            }
        }
    }

    GIVEN("non-empty string")
    {
        // Edge case: leading/trailing spaces, punctuation, repeated Unicode emoji
        const wchar_t* testStr = L"  Hello, World!  \U0001F60A\U0001F60A ";
        kf::USimpleString str(testStr);

        WHEN("isEmpty() is called")
        {
            THEN("it returns false")
            {
                REQUIRE(!str.isEmpty());
            }
        }

        WHEN("byteLength() is called")
        {
            auto byteLength = str.byteLength();

            THEN("it returns the correct byte length")
            {
                REQUIRE(byteLength == wcslen(testStr) * sizeof(wchar_t));
            }
        }

        WHEN("maxByteLength() is called")
        {
            auto maxByteLength = str.maxByteLength();

            THEN("it returns at least the current byte length")
            {
                REQUIRE(maxByteLength >= str.byteLength());
            }
        }

        WHEN("charLength() is called")
        {
            auto charLength = str.charLength();

            THEN("it returns the correct character length")
            {
                REQUIRE(charLength == wcslen(testStr));
            }
        }

        WHEN("maxCharLength() is called")
        {
            auto maxCharLength = str.maxCharLength();

            THEN("it returns at least the current char length")
            {
                REQUIRE(maxCharLength >= str.charLength());
            }
        }

        WHEN("setString is called with another non-empty string")
        {
            const wchar_t* newStr = L"Test123!@#";
            str.setString(newStr);

            THEN("string() now holds new pointer")
            {
                REQUIRE(str.string().Buffer == newStr);
            }

            THEN("charLength() returns the new character length")
            {
                REQUIRE(str.charLength() == wcslen(newStr));
            }

            THEN("maxCharLength() takes into account the terminating zero")
            {
                REQUIRE(str.maxCharLength() == str.charLength() + 1);
            }
            
            THEN("byteLength() returns the new byte length")
            {
                REQUIRE(str.byteLength() == wcslen(newStr) * sizeof(wchar_t));
            }

            THEN("maxByteLength() takes into account the terminating zero")
            {
                REQUIRE(str.maxByteLength() == str.byteLength() + sizeof(wchar_t));
            }
        }

        WHEN("begin() is called")
        {
            THEN("it returns a pointer to the first character")
            {
                REQUIRE(str.begin() == str.buffer());
                REQUIRE(*str.begin() == testStr[0]);
            }
        }

        WHEN("end() is called")
        {
            THEN("it returns a pointer to one past the last character")
            {
                REQUIRE(str.end() == str.buffer() + str.charLength());
            }
        }

        WHEN("buffer() is called")
        {
            THEN("it returns a pointer to the string data")
            {
                REQUIRE(str.buffer() == testStr);
            }
        }

        WHEN("compareTo() is called with same string")
        {
            kf::USimpleString sameStr(L"  Hello, World!  \U0001F60A\U0001F60A ");
            auto compareResult = str.compareTo(sameStr.string());

            THEN("it returns 0")
            {
                REQUIRE(compareResult == 0);
            }
        }

        WHEN("compareTo() is called with lexicographically greater string")
        {
            kf::USimpleString diffStr(L"zzz");
            auto compareResult = str.compareTo(diffStr.string());

            THEN("it returns a negative value")
            {
                REQUIRE(compareResult < 0);
            }
        }

        WHEN("compareTo() is called with lexicographically smaller string")
        {
            kf::USimpleString diffStr(L"   abc");
            auto compareResult = str.compareTo(diffStr.string());

            THEN("it returns a positive value")
            {
                REQUIRE(compareResult > 0);
            }
        }

        WHEN("compareToIgnoreCase() is called with same string, different case")
        {
            kf::USimpleString sameStr(L"  hello, world!  \U0001F60A\U0001F60A ");

            THEN("it returns 0")
            {
                REQUIRE(str.compareToIgnoreCase(sameStr.string()) == 0);
            }
        }

        WHEN("equals() is called with same string")
        {
            kf::USimpleString sameStr(testStr);

            THEN("it returns true")
            {
                REQUIRE(str.equals(sameStr.string()));
            }
        }

        WHEN("equals() is called with different string")
        {
            kf::USimpleString diffStr(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.equals(diffStr.string()));
            }
        }

        WHEN("equalsIgnoreCase() is called with same string, different case")
        {
            kf::USimpleString sameStr(L"  hello, world!  \U0001F60A\U0001F60A ");

            THEN("it returns true")
            {
                REQUIRE(str.equalsIgnoreCase(sameStr.string()));
            }
        }

        WHEN("operator< is called with lexicographically greater string")
        {
            kf::USimpleString greaterStr(L"zzz");

            THEN("it returns true")
            {
                REQUIRE(str < greaterStr);
            }
        }

        WHEN("operator< is called with lexicographically smaller string")
        {
            kf::USimpleString smallerStr(L"   aaa");

            THEN("it returns false")
            {
                REQUIRE(!(str < smallerStr));
            }
        }

        WHEN("operator== is called with same string")
        {
            kf::USimpleString sameStr(testStr);

            THEN("it returns true")
            {
                REQUIRE(str == sameStr);
            }
        }

        WHEN("operator== is called with different string")
        {
            kf::USimpleString diffStr(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!(str == diffStr));
            }
        }

        WHEN("split() is called with present separator")
        {
            int fromIndex = 0;
            kf::USimpleString result = str.split(L',', fromIndex);

            THEN("it returns the substring before the separator")
            {
                REQUIRE(result.equals(L"  Hello"));
            }

            THEN("fromIndex is updated to the index after the separator")
            {
                REQUIRE(fromIndex == 8); // "  Hello,"
            }
        }

        WHEN("split() is called with absent separator")
        {
            int fromIndex = 0;
            kf::USimpleString result = str.split(L'|', fromIndex);

            THEN("it returns an original string")
            {
                REQUIRE(result.equals(testStr));
            }

            THEN("fromIndex is -1")
            {
                REQUIRE(fromIndex == -1);
            }
        }

        WHEN("contains() is called with present substring")
        {
            kf::USimpleString substring(L"World");

            THEN("it returns true")
            {
                REQUIRE(str.contains(substring));
            }
        }

        WHEN("contains() is called with absent substring")
        {
            kf::USimpleString substring(L"NotFound");

            THEN("it returns false")
            {
                REQUIRE(!str.contains(substring));
            }
        }

        WHEN("indexOf() is called with present character")
        {
            int index = str.indexOf(L'W');

            THEN("it returns the index of the first occurrence")
            {
                REQUIRE(index == 9); // "  Hello, World!  \U0001F60A\U0001F60A "
            }
        }

        WHEN("indexOf() is called with absent character")
        {
            int index = str.indexOf(L'X');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("lastIndexOf is called with present character")
        {
            int index = str.lastIndexOf(L' ');

            THEN("it returns the index of the last occurrence")
            {
                REQUIRE(index == str.charLength() - 1);
            }
        }

        WHEN("lastIndexOf is called with absent character")
        {
            int index = str.lastIndexOf(L'Q');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("substring() is called with only start index")
        {
            kf::USimpleString subStr = str.substring(2); // Should skip leading spaces

            THEN("it returns the substring from start index to end")
            {
                REQUIRE(subStr.equals(L"Hello, World!  \U0001F60A\U0001F60A "));
            }
        }

        WHEN("substring() is called with valid indices")
        {
            kf::USimpleString subStr = str.substring(2, 7); // "Hello"

            THEN("it returns the correct substring")
            {
                REQUIRE(subStr.equals(L"Hello"));
            }
        }

        // TODO: WHEN("substring() is called with invalid indices")

        WHEN("trim() is called with an absent character")
        {
            kf::USimpleString trimmedStr = str.trim(L'X');

            THEN("it returns the original string unchanged")
            {
                REQUIRE(trimmedStr.equals(testStr));
            }
        }

        WHEN("trim() is called with a present character")
        {
            kf::USimpleString trimmedStr = str.trim(L' ');

            THEN("it returns the string without leading and trailing spaces")
            {
                REQUIRE(trimmedStr.equals(L"Hello, World!  \U0001F60A\U0001F60A"));
            }
        }

        WHEN("trimRight() is called with an absent character")
        {
            kf::USimpleString trimmedStr = str.trimRight(L'X');

            THEN("it returns the original string unchanged")
            {
                REQUIRE(trimmedStr.equals(testStr));
            }
        }

        WHEN("trimRight() is called with a present character")
        {
            kf::USimpleString trimmedStr = str.trimRight(L' ');

            THEN("it returns the string without trailing spaces")
            {
                REQUIRE(trimmedStr.equals(L"  Hello, World!  \U0001F60A\U0001F60A"));
            }
        }

        WHEN("trimLeft() is called with an absent character")
        {
            kf::USimpleString trimmedStr = str.trimLeft(L'Y');

            THEN("it returns the original string unchanged")
            {
                REQUIRE(trimmedStr.equals(testStr));
            }
        }

        WHEN("trimLeft() is called with a present character")
        {
            kf::USimpleString trimmedStr = str.trimLeft(L' ');

            THEN("it returns the string without leading spaces")
            {
                REQUIRE(trimmedStr.equals(L"Hello, World!  \U0001F60A\U0001F60A "));
            }
        }

        WHEN("startsWith() is called with existing substring")
        {
            kf::USimpleString prefix(L"  Hello");

            THEN("it returns true")
            {
                REQUIRE(str.startsWith(prefix));
            }
        }

        WHEN("startsWith() is called with non-existing substring")
        {
            kf::USimpleString prefix(L"World");

            THEN("it returns false")
            {
                REQUIRE(!str.startsWith(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with same string, different case")
        {
            kf::USimpleString prefix(L"  hello");

            THEN("it returns true")
            {
                REQUIRE(str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("endsWith() is called with existing substring")
        {
            kf::USimpleString suffix(L"\U0001F60A ");

            THEN("it returns true")
            {
                REQUIRE(str.endsWith(suffix));
            }
        }

        WHEN("endsWith() is called with non-existing substring")
        {
            kf::USimpleString suffix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.endsWith(suffix));
            }
        }

        WHEN("endsWithIgnoreCase() is called with same string, different case")
        {
            kf::USimpleString suffix(L"\U0001F60A ");

            THEN("it returns true")
            {
                REQUIRE(str.endsWithIgnoreCase(suffix));
            }
        }
    }
}