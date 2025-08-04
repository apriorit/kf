#include "pch.h"
#include <kf/USimpleString.h>

SCENARIO("USimpleString::matches")
{
    GIVEN("A pattern with '*' wildcard")
    {
        kf::USimpleString pattern = L"*.txt";

        WHEN("The string matches the pattern")
        {
            kf::USimpleString matchingStr = L"example.txt";
            kf::USimpleString matchingStr2 = L"other-example123.txt";

            THEN("Strings match the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
                REQUIRE(matchingStr2.matches(pattern));
            }
        }

        WHEN("The string does not match the pattern")
        {
            kf::USimpleString nonMatchingStr = L"non-matching.exe";

            THEN("The string does not match")
            {
                REQUIRE(!nonMatchingStr.matches(pattern));
            }
        }
    }

    GIVEN("A pattern with '?' wildcard")
    {
        kf::USimpleString pattern = L"e?amp?e.txt";

        WHEN("The string matches the pattern")
        {
            kf::USimpleString matchingStr = L"example.txt";
            kf::USimpleString matchingStr2 = L"eeamppe.txt";

            THEN("The string matches the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
                REQUIRE(matchingStr2.matches(pattern));
            }
        }

        WHEN("The string does not match the pattern")
        {
            kf::USimpleString nonMatchingStr = L"nonExample.exe";

            THEN("The string does not match")
            {
                REQUIRE(!nonMatchingStr.matches(pattern));
            }
        }
    }

    GIVEN("A pattern with mixed '*' and '?' wildcards")
    {
        kf::USimpleString pattern = L"?xample*.txt";

        WHEN("The string matches the pattern")
        {
            kf::USimpleString matchingStr = L"xxample_123.txt";

            THEN("The string matches the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
            }
        }

        WHEN("The string does not match the pattern")
        {
            kf::USimpleString str = L"non-example.exe";

            THEN("The string does not match")
            {
                REQUIRE(!str.matches(pattern));
            }
        }
    }

    WHEN("Empty pattern and empty string")
    {
        kf::USimpleString pattern = L"";
        kf::USimpleString str = L"";

        THEN("They match")
        {
            REQUIRE(str.matches(pattern));
        }
    }

    WHEN("Pattern is '*' and string is empty")
    {
        kf::USimpleString pattern = L"*";
        kf::USimpleString str = L"";

        THEN("'*' does not match an empty string")
        {
            REQUIRE(!str.matches(pattern));
        }
    }

    WHEN("Pattern is empty and string is not")
    {
        kf::USimpleString pattern = L"";
        kf::USimpleString str = L"file.txt";

        THEN("They do not match")
        {
            REQUIRE(!str.matches(pattern));
        }
    }

    WHEN("The string has different case")
    {
        kf::USimpleString pattern = L"example.txt";

        kf::USimpleString str1 = L"Example.txt";
        kf::USimpleString str2 = L"EXAMPLE.TXT";
        kf::USimpleString str3 = L"eXample.TxT";

        THEN("They do not match")
        {
            REQUIRE(!str1.matches(pattern));
            REQUIRE(!str2.matches(pattern));
            REQUIRE(!str3.matches(pattern));
        }
    }
}
