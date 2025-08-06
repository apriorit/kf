#include "pch.h"
#include <kf/USimpleString.h>

SCENARIO("USimpleString::matches")
{
    GIVEN("A pattern with '*' wildcard")
    {
        WHEN("'*' at the beginning")
        {
            kf::USimpleString pattern(L"*.txt");
            kf::USimpleString matchingStr(L"example.txt");
            kf::USimpleString matchingStr2(L"other-example123.txt");
            kf::USimpleString matchingStr3(L"file.doc");

            THEN("example.txt and other-example123.txt match the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
                REQUIRE(matchingStr2.matches(pattern));
            }

            THEN("file.doc doesn't match the pattern")
            {
                REQUIRE(!matchingStr3.matches(pattern));
            }
        }

        WHEN("'*' in the middle")
        {
            kf::USimpleString pattern(L"ex*.txt");
            kf::USimpleString matchingStr(L"example.txt");
            kf::USimpleString nonMatching(L"other-example123.txt");

            THEN("example.txt matches the pattern ex*.txt")
            {
                REQUIRE(matchingStr.matches(pattern));
            }

            THEN("other-example123.txt doesn't match the pattern ex*.txt")
            {
                REQUIRE(!nonMatching.matches(pattern));
            }
        }

        WHEN("'*' in the end")
        {
            kf::USimpleString pattern(L"example*");
            kf::USimpleString matchingStr(L"example.txt");
            kf::USimpleString nonMatching(L"other-example123.txt");

            THEN("example.txt matches the pattern example*")
            {
                REQUIRE(matchingStr.matches(pattern));
            }

            THEN("other-example123.txt doesn't match the pattern example*")
            {
                REQUIRE(!nonMatching.matches(pattern));
            }
        }

        WHEN("'*' at the beginning, in the middle and in the end")
        {
            kf::USimpleString pattern(L"*ex*le*");
            kf::USimpleString matchingStr(L"example.txt");
            kf::USimpleString matchingStr2(L"other-example123.txt");
            kf::USimpleString nonMatching(L"file.csv");

            THEN("example.txt and other-example123.txt match the pattern *ex*le*")
            {
                REQUIRE(matchingStr.matches(pattern));
                REQUIRE(matchingStr2.matches(pattern));
            }

            THEN("file.csv doesn't match the pattern *ex*le*")
            {
                REQUIRE(!nonMatching.matches(pattern));
            }
        }
    }

    GIVEN("A pattern with '?' wildcard")
    {
        WHEN("'?' at the beginning")
        {
            kf::USimpleString pattern(L"?xample.txt");
            kf::USimpleString matchingStr (L"example.txt");
            kf::USimpleString nonMatchingStr (L"eeamppe.txt");

            THEN("example.txt matches the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
            }

            THEN("eeamppe.txt does not match the pattern")
            {
                REQUIRE(!nonMatchingStr.matches(pattern));
            }
        }

        GIVEN("A pattern with '?' wildcard")
        {
            WHEN("'?' at the beginning")
            {
                kf::USimpleString pattern(L"?xample.txt");
                kf::USimpleString matchingStr(L"example.txt");
                kf::USimpleString nonMatchingStr(L"eeamppe.txt");

                THEN("example.txt matches the pattern ?xample.txt")
                {
                    REQUIRE(matchingStr.matches(pattern));
                }

                THEN("eeamppe.txt does not match the pattern ?xample.txt")
                {
                    REQUIRE(!nonMatchingStr.matches(pattern));
                }
            }

            WHEN("'?' in the middle")
            {
                kf::USimpleString pattern(L"exa?ple.txt");
                kf::USimpleString matchingStr(L"example.txt");
                kf::USimpleString nonMatchingStr(L"eeamppe.txt");

                THEN("example.txt matches the pattern exa?ple.txt")
                {
                    REQUIRE(matchingStr.matches(pattern));
                }

                THEN("eeamppe.txt does not match the pattern exa?ple.txt")
                {
                    REQUIRE(!nonMatchingStr.matches(pattern));
                }
            }

            WHEN("'?' in the end")
            {
                kf::USimpleString pattern(L"example.tx?");
                kf::USimpleString matchingStr(L"example.txt");
                kf::USimpleString nonMatchingStr(L"eeamppe.txt");

                THEN("example.txt matches the pattern example.tx?")
                {
                    REQUIRE(matchingStr.matches(pattern));
                }

                THEN("eeamppe.txt does not match the pattern example.tx?")
                {
                    REQUIRE(!nonMatchingStr.matches(pattern));
                }
            }
        }
    }

    GIVEN("A pattern with mixed '*' and '?' wildcards")
    {
        kf::USimpleString pattern (L"?xample*.txt");

        WHEN("The string matches the pattern")
        {
            kf::USimpleString matchingStr (L"xxample_123.txt");

            THEN("The string matches the pattern")
            {
                REQUIRE(matchingStr.matches(pattern));
            }
        }

        WHEN("The string does not match the pattern")
        {
            kf::USimpleString str (L"non-example.exe");

            THEN("The string does not match")
            {
                REQUIRE(!str.matches(pattern));
            }
        }
    }

    WHEN("Empty pattern and empty string")
    {
        kf::USimpleString pattern (L"");
        kf::USimpleString str (L"");

        THEN("They match")
        {
            REQUIRE(str.matches(pattern));
        }
    }

    WHEN("Pattern is '*' and string is empty")
    {
        kf::USimpleString pattern (L"*");
        kf::USimpleString str (L"");

        THEN("'*' does not match an empty string")
        {
            REQUIRE(!str.matches(pattern));
        }
    }

    WHEN("Pattern is empty and string is not")
    {
        kf::USimpleString pattern (L"");
        kf::USimpleString str (L"file.txt");

        THEN("They do not match")
        {
            REQUIRE(!str.matches(pattern));
        }
    }

    WHEN("The string has different case")
    {
        kf::USimpleString pattern (L"example.txt");

        kf::USimpleString str1 (L"Example.txt");
        kf::USimpleString str2 (L"EXAMPLE.TXT");
        kf::USimpleString str3 (L"eXample.TxT");

        THEN("They do not match")
        {
            REQUIRE(!str1.matches(pattern));
            REQUIRE(!str2.matches(pattern));
            REQUIRE(!str3.matches(pattern));
        }
    }
}

SCENARIO("USimpleString::matchesIgnoreCase")
{
    GIVEN("A pattern with '*' wildcard")
    {
        kf::USimpleString pattern(L"*.TXT");

        kf::USimpleString matchingStr (L"EXAMPLE.TXT");
        kf::USimpleString matchingStr2 (L"Example.txt");
        kf::USimpleString matchingStr3 (L"example.txt");
        kf::USimpleString nonMatchingStr (L"NoN-matching.ExE");

        THEN("EXAMPLE.TXT, Example.txt, example.txt match the pattern *.txt")
        {
            REQUIRE(matchingStr.matchesIgnoreCase(pattern));
            REQUIRE(matchingStr2.matchesIgnoreCase(pattern));
            REQUIRE(matchingStr3.matchesIgnoreCase(pattern));
        }

        THEN("NoN-matching.ExE does not match *.txt")
        {
            REQUIRE(!nonMatchingStr.matchesIgnoreCase(pattern));
        }
    }

    GIVEN("A pattern with '?' wildcard")
    {
        kf::USimpleString pattern (L"E?AMP?E.TXT");

        kf::USimpleString matchingStr (L"EXAMPLE.txt");
        kf::USimpleString matchingStr2 (L"eeamppe.txt");
        kf::USimpleString matchingStr3 (L"eXampLe.TXT");
        kf::USimpleString nonMatchingStr (L"Example.EXE");

        THEN("EXAMPLE.txt, eeamppe.txt, eXampLe.TXT match the pattern e?amp?e.txt")
        {
            REQUIRE(matchingStr.matchesIgnoreCase(pattern));
            REQUIRE(matchingStr2.matchesIgnoreCase(pattern));
            REQUIRE(matchingStr3.matchesIgnoreCase(pattern));
        }

        THEN("Example.EXE does not match e?amp?e.txt")
        {
            REQUIRE(!nonMatchingStr.matchesIgnoreCase(pattern));
        }
    }

    GIVEN("A pattern with mixed '*' and '?' wildcards")
    {
        kf::USimpleString pattern (L"?XAMPLE*.TXT");
        kf::USimpleString matchingStr (L"xxample_123.txt");
        kf::USimpleString matchingStr2 (L"xxAMPle_123.TXT");
        kf::USimpleString str (L"NoN-Example.exe");

        THEN("xxample_123.txt, xxAMPle_123.TXT match the pattern ?xample*.txt")
        {
            REQUIRE(matchingStr.matchesIgnoreCase(pattern));
            REQUIRE(matchingStr2.matchesIgnoreCase(pattern));
        }

        THEN("NoN-Example.exe does not match the pattern ?xample*.txt")
        {
            REQUIRE(!str.matchesIgnoreCase(pattern));
        }
    }

    WHEN("Pattern is empty and string is not")
    {
        kf::USimpleString pattern (L"");
        kf::USimpleString str (L"file.txt");
        kf::USimpleString str2 (L"FILE.TXT");

        THEN("They do not match")
        {
            REQUIRE(!str.matchesIgnoreCase(pattern));
            REQUIRE(!str2.matchesIgnoreCase(pattern));
        }
    }
}
