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

        WHEN("setString is called with a non-empty string")
        {
            str.setString(L"Hello, World!");

            THEN("isEmpty() returns false")
            {
                REQUIRE(!str.isEmpty());
            }

            THEN("byteLength() is not null")
            {
                REQUIRE(str.byteLength() != 0);
            }

            THEN("charLength() is not null")
            {
                REQUIRE(str.charLength() != 0);
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

        WHEN("split() is called with absent separator")
        {
            int fromIndex = 0;
            kf::USimpleString result = str.split(L',', fromIndex);
            THEN("it returns an empty string")
            {
                REQUIRE(result.isEmpty());
                REQUIRE(fromIndex == -1);
            }
        }

        WHEN("split() is called with present separator")
        {
            str.setString(L"Hello,World");
            int fromIndex = 0;
            kf::USimpleString result = str.split(L',', fromIndex);
            THEN("it returns the substring before the separator")
            {
                REQUIRE(result.equals(L"Hello"));
                REQUIRE(fromIndex == 6); // Length of "Hello,"
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

        WHEN("contains() is called with absent substring")
        {
            kf::USimpleString substring(L"Test");

            THEN("it returns false")
            {
                REQUIRE(!str.contains(substring));
            }
        }

        WHEN("contains() is called with present substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString substring(L"World");

            THEN("it returns true")
            {
                REQUIRE(str.contains(substring));
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

        WHEN("indexOf() is called with present character")
        {
            str.setString(L"Hello, World!");
            int index = str.indexOf(L'o');

            THEN("it returns the index of the first occurrence")
            {
                REQUIRE(index == 4); // 'o' in "Hello"
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

        WHEN("lastIndexOf is called with absent character")
        {
            int index = str.lastIndexOf(L'Y');

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("lastIndexOf is called with present character")
        {
            str.setString(L"Hello, World!");
            int index = str.lastIndexOf(L'o');

            THEN("it returns the index of the last occurrence")
            {
                REQUIRE(index == 8); // 'o' in "World"
            }
        }

        WHEN("substring() is called with only start index")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString subStr = str.substring(7); // "World!"
            THEN("it returns the substring from start index to end")
            {
                REQUIRE(subStr.equals(L"World!"));
            }
        }

        WHEN("substring() is called with valid indices")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString subStr = str.substring(7, 12); // "World"

            THEN("it returns the correct substring")
            {
                REQUIRE(subStr.equals(L"World"));
            }
        }

        WHEN("substring() is called with invalid indices")
        {
            str.setString(L"Hello, World!");

            THEN("it returns empty substring")
            {
                REQUIRE(str.substring(12, 5).isEmpty());
            }
        }

        WHEN("trim() is called with a present character")
        {
            str.setString(L"   Hello   ");
            kf::USimpleString trimmedStr = str.trim(L' ');

            THEN("it returns the string without leading and trailing spaces")
            {
                REQUIRE(trimmedStr.equals(L"Hello"));
            }
        }

        WHEN("trimRight() is called with a present character")
        {
            str.setString(L"Hello   ");
            kf::USimpleString trimmedStr = str.trimRight(L' ');

            THEN("it returns the string without trailing spaces")
            {
                REQUIRE(trimmedStr.equals(L"Hello"));
            }
        }

        WHEN("trimRight() is called with an absent character")
        {
            str.setString(L"Hello - 112");
            kf::USimpleString trimmedStr = str.trimRight(L' ');

            THEN("it returns the original string")
            {
                REQUIRE(trimmedStr.equals(L"Hello - 112"));
            }
        }

        WHEN("trimLeft() is called with a character")
        {
            str.setString(L"   Hello");
            kf::USimpleString trimmedStr = str.trimLeft(L' ');

            THEN("it returns the string without leading spaces")
            {
                REQUIRE(trimmedStr.equals(L"Hello"));
            }
        }

        WHEN("trimLeft() is called with an absent character")
        {
            str.setString(L"221 - Hello");
            kf::USimpleString trimmedStr = str.trimRight(L' ');

            THEN("it returns the original string")
            {
                REQUIRE(trimmedStr.equals(L"221 - Hello"));
            }
        }

        WHEN("startsWith() is called with existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString prefix(L"Hello");

            THEN("it returns true")
            {
                REQUIRE(str.startsWith(prefix));
            }
        }

        WHEN("startsWith() is called with non-existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString prefix(L"World");

            THEN("it returns false")
            {
                REQUIRE(!str.startsWith(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with exact substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString prefix(L"hello");

            THEN("it returns true")
            {
                REQUIRE(str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with existing substring and with another case")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString prefix(L"HELLO");
            THEN("it returns true")
            {
                REQUIRE(str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("startsWithIgnoreCase() is called with non-existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString prefix(L"WORLD");

            THEN("it returns false")
            {
                REQUIRE(!str.startsWithIgnoreCase(prefix));
            }
        }

        WHEN("endsWith() is called with existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"World!");

            THEN("it returns true")
            {
                REQUIRE(str.endsWith(suffix));
            }
        }

        WHEN("endsWith() is called with existing substring and with another case")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"WORLD!");

            THEN("it returns true")
            {
                REQUIRE(str.endsWithIgnoreCase(suffix));
            }
        }

        WHEN("endsWith() is called with non-existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.endsWith(suffix));
            }
        }

        WHEN("endsWithIgnoreCase() is called with existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"WORLD!");

            THEN("it returns true")
            {
                REQUIRE(str.endsWithIgnoreCase(suffix));
            }
        }
        
        WHEN("endsWithIgnoreCase() is called with existing substring and with another case")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"world!");

            THEN("it returns true")
            {
                REQUIRE(str.endsWithIgnoreCase(suffix));
            }
        }

        WHEN("endsWithIgnoreCase() is called with non-existing substring")
        {
            str.setString(L"Hello, World!");
            kf::USimpleString suffix(L"hello");

            THEN("it returns false")
            {
                REQUIRE(!str.endsWithIgnoreCase(suffix));
            }
        }
    }
}