#include "pch.h"
#include <kf/ASimpleString.h>

SCENARIO("ASimpleString: all methods")
{
    GIVEN("Empty string")
    {
        kf::ASimpleString str;

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

        WHEN("charLength() is called")
        {
            THEN("it returns 0")
            {
                REQUIRE(str.charLength() == 0);
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

        WHEN("equals() is called with empty string")
        {
            kf::ASimpleString emptyStr;

            THEN("it returns true")
            {
                REQUIRE(str.equals(emptyStr.string()));
            }
        }

        WHEN("equals() is called with non-empty string")
        {
            kf::ASimpleString nonEmptyStr("Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.equals(nonEmptyStr.string()));
            }
        }

        WHEN("contains() is called with empty substring")
        {
            kf::ASimpleString substring;

            THEN("it returns true")
            {
                REQUIRE(str.contains(substring));
            }
        }

        WHEN("contains() is called with non-empty substring")
        {
            kf::ASimpleString substring("Test");

            THEN("it returns false")
            {
                REQUIRE(!str.contains(substring));
            }
        }

        WHEN("indexOf(char) is called")
        {
            int index = str.indexOf('X', 0);

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("split() is called")
        {
            int fromIndex = 0;
            kf::ASimpleString result = str.split(',', fromIndex);

            THEN("it returns an empty string")
            {
                REQUIRE(result.isEmpty());
                REQUIRE(fromIndex == -1);
            }
        }

        WHEN("trimRight() is called")
        {
            kf::ASimpleString trimmedStr = str.trimRight(' ');

            THEN("it returns an empty string")
            {
                REQUIRE(trimmedStr.isEmpty());
            }
        }

        WHEN("trimLeft() is called")
        {
            kf::ASimpleString trimmedStr = str.trimLeft(' ');

            THEN("it returns an empty string")
            {
                REQUIRE(trimmedStr.isEmpty());
            }
        }
    }

    GIVEN("non-empty string")
    {
        const char* testStr = "  Hello, World!  :):) ";
        kf::ASimpleString str(testStr);

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
                REQUIRE(byteLength == strlen(testStr));
            }
        }

        WHEN("charLength() is called")
        {
            auto charLength = str.charLength();

            THEN("it returns the correct character length")
            {
                REQUIRE(charLength == strlen(testStr));
            }
        }

        WHEN("setString is called with another non-empty string")
        {
            const char* newStr = "Test123!@#";
            str.setString(const_cast<char*>(newStr), static_cast<int>(strlen(newStr)));

            THEN("string() now holds new pointer")
            {
                REQUIRE(str.string().Buffer == newStr);
            }

            THEN("charLength() returns the new character length")
            {
                REQUIRE(str.charLength() == strlen(newStr));
            }

            THEN("byteLength() returns the new byte length")
            {
                REQUIRE(str.byteLength() == strlen(newStr));
            }
        }

        WHEN("begin() is called")
        {
            THEN("it returns a pointer to the first character")
            {
                REQUIRE(str.begin() == str.string().Buffer);
                REQUIRE(*str.begin() == testStr[0]);
            }
        }

        WHEN("end() is called")
        {
            THEN("it returns a pointer to one past the last character")
            {
                REQUIRE(str.end() == str.begin() + str.charLength());
            }
        }

        WHEN("equals() is called with same string")
        {
            kf::ASimpleString sameStr(testStr);

            THEN("it returns true")
            {
                REQUIRE(str.equals(sameStr.string()));
            }
        }

        WHEN("equals() is called with different string")
        {
            kf::ASimpleString diffStr("Hello");

            THEN("it returns false")
            {
                REQUIRE(!str.equals(diffStr.string()));
            }
        }

        WHEN("split() is called with present separator")
        {
            int fromIndex = 0;
            kf::ASimpleString result = str.split(',', fromIndex);
            kf::ASimpleString expected("  Hello");

            THEN("it returns the substring before the separator")
            {
                REQUIRE(result.equals(expected.string()));
            }

            THEN("fromIndex is updated to the index after the separator")
            {
                REQUIRE(fromIndex == 8); // "  Hello,"
            }
        }

        WHEN("split() is called with absent separator")
        {
            int fromIndex = 0;
            kf::ASimpleString result = str.split('|', fromIndex);
            kf::ASimpleString expected(testStr);

            THEN("it returns an original string")
            {
                REQUIRE(result.equals(expected.string()));
            }

            THEN("fromIndex is -1")
            {
                REQUIRE(fromIndex == -1);
            }
        }

        WHEN("contains() is called with present substring")
        {
            kf::ASimpleString substring("World");

            THEN("it returns true")
            {
                REQUIRE(str.contains(substring));
            }
        }

        WHEN("contains() is called with absent substring")
        {
            kf::ASimpleString substring("NotFound");

            THEN("it returns false")
            {
                REQUIRE(!str.contains(substring));
            }
        }

        WHEN("indexOf(char) is called with present character")
        {
            int index = str.indexOf('W', 0);

            THEN("it returns the index of the first occurrence")
            {
                REQUIRE(index == 9); // "  Hello, World!  :):) "
            }
        }

        WHEN("indexOf(char) is called with absent character")
        {
            int index = str.indexOf('X', 0);

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("indexOf(substring) is called with present substring")
        {
            kf::ASimpleString sub("World");
            int index = str.indexOf(sub);

            THEN("it returns the index of the first occurrence")
            {
                REQUIRE(index == 9);
            }
        }

        WHEN("indexOf(substring, fromIndex) is called after the match")
        {
            kf::ASimpleString sub("World");
            int index = str.indexOf(sub, 10);

            THEN("it returns -1")
            {
                REQUIRE(index == -1);
            }
        }

        WHEN("substring() is called with only start index")
        {
            kf::ASimpleString subStr = str.substring(2);
            kf::ASimpleString expected("Hello, World!  :):) ");

            THEN("it returns the substring from start index to end")
            {
                REQUIRE(subStr.equals(expected.string()));
            }
        }

        WHEN("substring() is called with valid indices")
        {
            kf::ASimpleString subStr = str.substring(2, 7);
            kf::ASimpleString expected("Hello");

            THEN("it returns the correct substring")
            {
                REQUIRE(subStr.equals(expected.string()));
            }
        }

        WHEN("trimRight() is called with an absent character")
        {
            kf::ASimpleString trimmedStr = str.trimRight('X');
            kf::ASimpleString expected(testStr);

            THEN("it returns the original string unchanged")
            {
                REQUIRE(trimmedStr.equals(expected.string()));
            }
        }

        WHEN("trimRight() is called with a present character")
        {
            kf::ASimpleString trimmedStr = str.trimRight(' ');
            kf::ASimpleString expected("  Hello, World!  :):)");

            THEN("it returns the string without trailing spaces")
            {
                REQUIRE(trimmedStr.equals(expected.string()));
            }
        }

        WHEN("trimLeft() is called with an absent character")
        {
            kf::ASimpleString trimmedStr = str.trimLeft('Y');
            kf::ASimpleString expected(testStr);

            THEN("it returns the original string unchanged")
            {
                REQUIRE(trimmedStr.equals(expected.string()));
            }
        }

        WHEN("trimLeft() is called with a present character")
        {
            kf::ASimpleString trimmedStr = str.trimLeft(' ');
            kf::ASimpleString expected("Hello, World!  :):) ");

            THEN("it returns the string without leading spaces")
            {
                REQUIRE(trimmedStr.equals(expected.string()));
            }
        }

        WHEN("trimLeft(chars) is called with multiple leading characters")
        {
            // Trim spaces then 'H'
            kf::ASimpleString chars(" H");
            kf::ASimpleString trimmedStr = str.trimLeft(chars);
            kf::ASimpleString expected("ello, World!  :):) ");

            THEN("it trims all leading characters present in the set")
            {
                REQUIRE(trimmedStr.equals(expected.string()));
            }
        }
    }
}