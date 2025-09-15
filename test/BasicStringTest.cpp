#include "pch.h"
#include <kf/stl/basic_string>

SCENARIO("basic_string: construction and assignment")
{
    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("Hello, World!"));

        WHEN("assign(size_type count, const T& value) is called")
        {
            auto status = str.assign(5, 'A');

            THEN("the string should contain the repeated character")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str == "AAAAA");
            }
        }

        WHEN("assign(const T* s) is called")
        {
            auto status = str.assign("New Content");

            THEN("the string should contain the new content")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "New Content");
            }
        }

        WHEN("assign(const T* s, size_type count) is called")
        {
            auto status = str.assign("Partial", 4);

            THEN("the string should contain the partial content")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str == "Part");
            }
        }

        WHEN("assign(const basic_string& other) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("Another String"));
            auto status = str.assign(other);

            THEN("the string should be assigned the content of the other string")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str == "Another String");
            }
        }

        WHEN("operator std::string_view() const is called")
        {
            std::string_view view(str);

            THEN("the view should reflect the string content")
            {
                REQUIRE(view == "Hello, World!");
            }
        }

        WHEN("swap(basic_string& other) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("Other"));
            str.swap(other);

            THEN("the contents should be swapped")
            {
                REQUIRE(str == "Other");
                REQUIRE(other == "Hello, World!");
            }
        }

        WHEN("assign(const T* s) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            auto status = wstr.assign(L"WideText");

            THEN("the wide string should contain the new content")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(wstr == L"WideText");
            }
        }

        WHEN("assign(T&& s) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr1;
            kf::basic_string<wchar_t, PagedPool> wstr2;
            REQUIRE_NT_SUCCESS(wstr1.assign(L"WideText"));

            auto status = wstr2.assign(std::move(wstr1));

            THEN("the wide string should contain the new content")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(wstr2 == L"WideText");
                REQUIRE(wstr1.empty());
            }
        }
    }

    GIVEN("Two basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("Hello, World!"));

        kf::basic_string<char, PagedPool> other;
        REQUIRE_NT_SUCCESS(other.assign("Other str"));

        WHEN("assign(const basic_string&, size_type, size_type) is called with pos > other.size()")
        {
            auto status = str.assign(other, 20, 5);

            THEN("the string should become empty")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str.empty());
            }
        }

        WHEN("assign(const basic_string&, size_type, size_type) is called with pos within range and count = npos")
        {
            auto status = str.assign(other, 3);

            THEN("the string should contain the suffix starting at pos")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str == "er str");
            }
        }

        WHEN("assign(const basic_string&, size_type, size_type) is called with pos within range and count < available")
        {
            auto status = str.assign(other, 2, 5);

            THEN("the string should contain the substring of given count")
            {
                REQUIRE(NT_SUCCESS(status));
                REQUIRE(str == "her s");
            }
        }
    }

    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("operator std::string_view() const is called")
        {
            std::string_view view(str);

            THEN("the view should be empty")
            {
                REQUIRE(view.empty());
            }
        }

        WHEN("assign(size_type count, const T& value) is called with zero count")
        {
            auto status = str.assign(0, 'Z');

            THEN("the string remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.empty());
            }
        }

        WHEN("assign(const T* s, size_type count) is called with zero count")
        {
            auto status = str.assign("abc", 0);

            THEN("the string remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.empty());
            }
        }

        WHEN("assign(const basic_string& other) is called with empty other")
        {
            kf::basic_string<char, PagedPool> other;
            auto status = str.assign(other);

            THEN("the string remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.empty());
            }
        }

        WHEN("swap(basic_string& other) is called with non-empty other")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("X"));
            str.swap(other);

            THEN("the empty string takes the content and other becomes empty")
            {
                REQUIRE(str == "X");
                REQUIRE(other.empty());
            }
        }
    }
}

SCENARIO("basic_string: Element access")
{
    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("abc"));

        WHEN("at(size_type pos) is called")
        {
            auto refOpt = str.at(1);

            THEN("it should be 'b' and writable")
            {
                REQUIRE(refOpt.has_value());
                REQUIRE(refOpt->get() == 'b');
                refOpt->get() = 'B';
                REQUIRE(str == "aBc");
            }
        }

        WHEN("at(size_type pos) is called on a non-const basic_string with out-of-range index")
        {
            auto refOpt = str.at(100);

            THEN("it should be empty")
            {
                REQUIRE(!refOpt.has_value());
            }
        }

        WHEN("at(size_type pos) is called on a const basic_string with out-of-range index")
        {
            const auto& cstr = str;
            auto refOpt = cstr.at(100);

            THEN("it should be empty")
            {
                REQUIRE(!refOpt.has_value());
            }
        }

        WHEN("operator[](size_type) is called")
        {
            THEN("the character is correct")
            {
                REQUIRE(str[0] == 'a');
                REQUIRE(str[1] == 'b');
                REQUIRE(str[2] == 'c');
            }
        }

        WHEN("operator[](size_type) is called for modifying")
        {
            str[2] = 'C';

            THEN("the characters should reflect updates via operator[]")
            {
                REQUIRE(str == "abC");
            }
        }

        WHEN("front() is called")
        {
            THEN("front should return the first character")
            {
                REQUIRE(str.front() == 'a');
            }
        }

        WHEN("back() is called")
        {
            THEN("back should return the last character")
            {
                REQUIRE(str.back() == 'c');
            }
        }

        WHEN("data() is called")
        {
            auto ptr = str.data();
            constexpr std::string_view kExpected = "abc";

            THEN("data should not be null")
            {
                REQUIRE(ptr != nullptr);
                REQUIRE(std::string_view(ptr) == kExpected);
            }
        }

        WHEN("c_str() is called")
        {
            THEN("c_str should not be null")
            {
                REQUIRE(str.c_str() != nullptr);
            }
        }

        WHEN("at(size_type pos) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            REQUIRE_NT_SUCCESS(wstr.assign(L"Some-long-string-for-testing-positions"));
            auto refOpt1 = wstr.at(1);
            auto refOpt2 = wstr.at(11);
            auto refOpt3 = wstr.at(26);
            auto refOpt4 = wstr.at(37);

            THEN("wide element should be accessible")
            {
                REQUIRE(refOpt1.has_value());
                REQUIRE(refOpt1->get() == L'o');

                REQUIRE(refOpt2.has_value());
                REQUIRE(refOpt2->get() == L't');

                REQUIRE(refOpt3.has_value());
                REQUIRE(refOpt3->get() == L'n');

                REQUIRE(refOpt4.has_value());
                REQUIRE(refOpt4->get() == L's');
            }
        }
    }

    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("at(size_type pos) is called")
        {
            auto refOpt = str.at(0);

            THEN("it should be empty")
            {
                REQUIRE(!refOpt.has_value());
            }
        }

        WHEN("data() is called")
        {
            THEN("data should not be null and point to a null terminator")
            {
                REQUIRE(str.data() != nullptr);
                REQUIRE(*str.data() == '\0');
            }
        }

        WHEN("c_str() is called")
        {
            THEN("c_str should not be null and be an empty string")
            {
                REQUIRE(str.c_str() != nullptr);
                REQUIRE(str.c_str()[0] == '\0');
            }
        }
    }
}

SCENARIO("basic_string: Iterators")
{
    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("abcd"));

        WHEN("begin() and end() are used to iterate")
        {
            std::array<char, 4> acc{};
            size_t i = 0;
            for (auto it = str.begin(); it != str.end(); ++it)
            {
                acc[i++] = *it;
            }

            THEN("accumulated sequence matches")
            {
                REQUIRE(acc[0] == 'a');
                REQUIRE(acc[1] == 'b');
                REQUIRE(acc[2] == 'c');
                REQUIRE(acc[3] == 'd');
            }
        }

        WHEN("rbegin() and rend() are used to iterate")
        {
            std::array<char, 4> acc{};
            size_t i = 0;
            for (auto it = str.rbegin(); it != str.rend(); ++it)
            {
                acc[i++] = *it;
            }

            THEN("accumulated sequence matches reverse")
            {
                REQUIRE(acc[0] == 'd');
                REQUIRE(acc[1] == 'c');
                REQUIRE(acc[2] == 'b');
                REQUIRE(acc[3] == 'a');
            }
        }

        WHEN("cbegin() and cend() are used to iterate over const basic_string")
        {
            const auto& cstr = str;
            std::array<char, 4> acc{};
            size_t i = 0;
            for (auto it = cstr.cbegin(); it != cstr.cend(); ++it)
            {
                acc[i++] = *it;
            }

            THEN("accumulated sequence matches")
            {
                REQUIRE(acc[0] == 'a');
                REQUIRE(acc[1] == 'b');
                REQUIRE(acc[2] == 'c');
                REQUIRE(acc[3] == 'd');
            }
        }
    }

    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("begin() and end() are used to iterate")
        {
            THEN("begin equals end for empty string")
            {
                REQUIRE(str.begin() == str.end());
            }
        }

        WHEN("rbegin() and rend() are used to iterate")
        {
            THEN("rbegin equals rend for empty string")
            {
                REQUIRE(str.rbegin() == str.rend());
            }
        }

        WHEN("cbegin() and cend() are used on a const basic_string")
        {
            const auto& cstr = str;
            THEN("cbegin equals cend for empty string")
            {
                REQUIRE(cstr.cbegin() == cstr.cend());
            }
        }
    }
}

SCENARIO("basic_string: Size and capacity")
{
    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE(str.empty());

        WHEN("reserve(size_type newCapacity) is called")
        {
            auto st1 = str.reserve(32);
            const auto cap = str.capacity();

            THEN("reserve should succeed and capacity grow")
            {
                REQUIRE_NT_SUCCESS(st1);
                REQUIRE(cap >= 32);
            }
        }

        WHEN("length() is called")
        {
            const auto length = str.length();
            const auto size = str.size();

            THEN("length and size are equal to 0")
            {
                REQUIRE(length == 0);
                REQUIRE(size == 0);
            }
        }

        WHEN("max_size() is called")
        {
            const auto max = str.max_size();
            const auto sz = str.size();

            THEN("max_size is sane")
            {
                REQUIRE(max >= sz);
            }
        }
    }

    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("abcdef"));

        WHEN("size() is called")
        {
            constexpr auto kExpectedSize = 6;

            THEN("size equals number of characters")
            {
                REQUIRE(str.size() == kExpectedSize);
            }
        }

        WHEN("length() is called")
        {
            constexpr auto kExpectedLength = 6;

            THEN("length equals number of characters")
            {
                REQUIRE(str.length() == kExpectedLength);
            }
        }

        WHEN("capacity() is called")
        {
            THEN("capacity is at least size")
            {
                REQUIRE(str.capacity() >= str.size());
            }
        }

        WHEN("reserve(size_type newCapacity) is called")
        {
            auto stRes = str.reserve(64);

            THEN("reserve succeeds and content preserved")
            {
                REQUIRE_NT_SUCCESS(stRes);
                REQUIRE(str.capacity() >= 64);
                REQUIRE(str == "abcdef");
            }
        }

        WHEN("shrink_to_fit() is called")
        {
            // shrink_to_fit has no effect on small strings stored in SSO (size <= 15 chars);
            constexpr size_t newCap = 64;
            REQUIRE_NT_SUCCESS(str.assign("1-2-3-4-5-6-7-8-9"));
            REQUIRE_NT_SUCCESS(str.reserve(newCap));
            auto status = str.shrink_to_fit();
            const auto capNow = str.capacity();

            THEN("capacity reduced")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(capNow < newCap);
                REQUIRE(str == "1-2-3-4-5-6-7-8-9");
            }
        }

        WHEN("resize(size_type count) is called")
        {
            auto status = str.resize(3);

            THEN("string should be truncated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abc");
            }
        }

        WHEN("resize(size_type count, const T& value) is called")
        {
            auto status = str.resize(9, 'x');

            THEN("the string should be grown with fill value")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abcdefxxx");
            }
        }

        WHEN("max_size() is called")
        {
            THEN("max_size is at least current size")
            {
                REQUIRE(str.max_size() >= str.size());
            }
        }
    }
}

SCENARIO("basic_string: Comparison")
{
    GIVEN("two basic_strings<char> with content")
    {
        kf::basic_string<char, PagedPool> a, b;
        REQUIRE_NT_SUCCESS(a.assign("alpha"));
        REQUIRE_NT_SUCCESS(b.assign("beta"));

        WHEN("operator==(const basic_string&) is called")
        {
            kf::basic_string<char, PagedPool> c;
            REQUIRE_NT_SUCCESS(c.assign("alpha"));

            THEN("they are equal when contents match")
            {
                REQUIRE(a == c);
            }
        }

        WHEN("operator!=(const basic_string&) is called")
        {
            THEN("they are not equal when contents differ")
            {
                REQUIRE(a != b);
            }
        }

        WHEN("operator<(const basic_string&) is called")
        {
            THEN("they compare lexicographically")
            {
                REQUIRE(a < b);
            }
        }

        WHEN("compare(const basic_string&) is called")
        {
            const auto cmp = a.compare(b);

            THEN("compare is negative when lhs < rhs")
            {
                REQUIRE(cmp < 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string& str) is called")
        {
            auto cmp = a.compare(1, 2, b);

            THEN("compare is positive when substring lhs > rhs")
            {
                REQUIRE(cmp > 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string&) with count1 > remaining length is called")
        {
            auto cmp = a.compare(3, 10, b);

            THEN("compare is positive")
            {
                REQUIRE(cmp > 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string& str) with empty substring of a is called")
        {
            auto cmp = a.compare(0, 0, b);

            THEN("empty substring is less than non-empty string")
            {
                REQUIRE(cmp < 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string& str, pos2, count2) is called")
        {
            auto cmp = a.compare(0, 2, b, 1, 2); // "al" vs "et"

            THEN("compare is negative when lhs < rhs")
            {
                REQUIRE(cmp < 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string& str, pos2, count2) for substrings of the same string is called")
        {
            auto cmp = a.compare(0, 2, a, 0, 2); // "al" vs "al"

            THEN("substrings are equal")
            {
                REQUIRE(cmp == 0);
            }
        }

        WHEN("compare(pos1, count1, const basic_string& str, pos2, count2) with overlapping positions is called")
        {
            auto cmp = a.compare(1, 3, a, 2, 3);

            THEN("comparison is negative")
            {
                REQUIRE(cmp < 0);
            }
        }

        WHEN("operator==(const T*) is called")
        {
            THEN("C-string equality works")
            {
                REQUIRE(a == "alpha");
            }
        }

        WHEN("operator!=(const T*) is called")
        {
            THEN("C-string inequality works")
            {
                REQUIRE(a != "alphabet");
            }
        }

        WHEN("operator<(const T*) is called")
        {
            THEN("C-string less-than works")
            {
                REQUIRE(a < "b");
            }
        }

        WHEN("compare(pos1, count1, const T* s) with C-style string is called")
        {
            auto cmp = a.compare(0, 2, "al"); // "al" == "al"

            THEN("substring equals C-style string")
            {
                REQUIRE(cmp == 0);
            }
        }

        WHEN("compare(pos1, count1, const T* s) with longer C-style string is called")
        {
            auto cmp = a.compare(0, 2, "alphabet"); // "al" < "alphabet"

            THEN("comparison is negative")
            {
                REQUIRE(cmp < 0);
            }
        }

        WHEN("compare(pos1, count1, const T* s, count2) with C-style string is called")
        {
            auto cmp = a.compare(0, 2, "alphabet", 2); // "al" == "al"

            THEN("substring equals first count2 chars of C-style string")
            {
                REQUIRE(cmp == 0);
            }
        }

        WHEN("compare(pos1, count1, const T* s, count2) with C-style string is called")
        {
            auto cmp = a.compare(2, 3, "alp", 2); // "pha" > "al"

            THEN("comparison is positive")
            {
                REQUIRE(cmp > 0);
            }
        }

        WHEN("compare(pos1, count1, const T* s, count2) with count2 > strlen(s)")
        {
            auto cmp = a.compare(0, 2, "al", 10);

            THEN("they are equal")
            {
                REQUIRE(cmp == 0);
            }
        }
    }

    GIVEN("basic_strings<wchar_t> with content")
    {
        kf::basic_string<wchar_t, PagedPool> w;
        REQUIRE_NT_SUCCESS(w.assign(L"zeta"));

        WHEN("operator==(const T*) is called on a wide string")
        {
            THEN("wide equality works with wide literals")
            {
                REQUIRE(w == L"zeta");
            }
        }

        WHEN("operator!=(const T*) is called on a wide string")
        {
            THEN("wide inequality works with wide literals")
            {
                REQUIRE(w != L"eta");
            }
        }

        WHEN("operator<(const T*) is called on a wide string")
        {
            THEN("wide less-than works with wide literals")
            {
                REQUIRE(w < L"zzzz");
            }
        }
    }

    GIVEN("one of basic_strings with content and one empty")
    {
        kf::basic_string<char, PagedPool> a, b;
        REQUIRE_NT_SUCCESS(a.assign("alpha"));
        // b is empty

        WHEN("operator==(const basic_string&) is called")
        {
            THEN("content string is not equal to empty string")
            {
                REQUIRE(a != b);
            }
        }

        WHEN("operator<(const basic_string&) is called")
        {
            THEN("empty string is less than content string")
            {
                REQUIRE(b < a);
            }
        }

        WHEN("compare(const basic_string&) is called")
        {
            const auto cmp1 = a.compare(b);
            const auto cmp2 = b.compare(a);

            THEN("compare reflects ordering vs empty")
            {
                REQUIRE(cmp1 > 0);
                REQUIRE(cmp2 < 0);
            }
        }

        WHEN("compare(pos1, count1, str) is called")
        {
            auto cmp = a.compare(0, 5, b);

            THEN("non-empty substring is greater than empty string")
            {
                REQUIRE(cmp > 0);
            }
        }

        WHEN("compare(pos1, count1, str) is called")
        {
            auto cmp = b.compare(0, 0, b);

            THEN("empty substrings are equal")
            {
                REQUIRE(cmp == 0);
            }
        }
    }
}

SCENARIO("basic_string: Modifiers")
{
    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("clear() is called")
        {
            str.clear();
            THEN("string remains empty")
            {
                REQUIRE(str.empty());
                REQUIRE(str.size() == 0);
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called")
        {
            auto it = str.erase(str.begin(), str.end());

            THEN("iterator points to begin/end")
            {
                REQUIRE(it == str.begin());
                REQUIRE(str.empty());
            }
        }

        WHEN("erase(const_iterator pos) is called")
        {
            auto it = str.erase(str.begin());

            THEN("no characters erased, iterator points to begin")
            {
                REQUIRE(it == str.begin());
                REQUIRE(str.empty());
            }
        }
    }

    GIVEN("basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("example"));

        WHEN("clear() is called")
        {
            str.clear();

            THEN("string is empty")
            {
                REQUIRE(str.empty());
                REQUIRE(str.size() == 0);
            }
        }

        WHEN("push_back(const T& value) is called")
        {
            const auto ch = 'd';
            auto status = str.push_back(ch);

            THEN("element is appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exampled");
            }
        }

        WHEN("push_back(T&& value) is called")
        {
            char ch = 'd';
            auto status = str.push_back(std::move(ch));

            THEN("element is appended using rvalue reference")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exampled");
            }
        }

        WHEN("pop_back() is called")
        {
            str.pop_back();

            THEN("last char removed")
            {
                REQUIRE(str == "exampl");
            }
        }

        WHEN("insert(const_iterator pos, const T& value) is called")
        {
            auto itOpt = str.insert(str.begin() + 2, 'X');

            THEN("iterator is returned and string modified")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "exXample");
            }
        }


        WHEN("insert(const_iterator pos, T&& value) is called")
        {
            char ch = 'X';
            auto itOpt = str.insert(str.begin() + 2, std::move(ch));

            THEN("iterator is returned and rvalue inserted")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "exXample");
            }
        }

        WHEN("insert(const_iterator pos, size_type count, const T& value) is called")
        {
            auto itOpt = str.insert(str.begin() + 2, 3, 'Y');

            THEN("multiple copies inserted at iterator position")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "exYYYample");
            }
        }

        WHEN("insert(size_type index, const T* s, size_type count) is called")
        {
            auto status = str.insert(1, "XYZ", 2);

            THEN("substring of C-string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "eXYxample");
            }
        }

        WHEN("insert(size_type index, size_type count, T ch) is called")
        {
            auto status = str.insert(1, 2, 'Y');

            THEN("characters inserted by count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "eYYxample");
            }
        }

        WHEN("insert(size_type index, const T* s) is called")
        {
            auto status = str.insert(0, "--");

            THEN("C-string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "--example");
            }
        }

        WHEN("insert(size_type index, const basic_string&) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("[]"));
            auto status = str.insert(2, other);

            THEN("basic_string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "ex[]ample");
            }
        }

        WHEN("insert(size_type index, const basic_string&, size_type index_str, size_type count) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("[]"));
            auto status = str.insert(2, other, 1, 1);

            THEN("substring from basic_string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "ex]ample");
            }
        }

        WHEN("insert(size_type index, const basic_string&, size_type index_str, size_type count) with index_str > str.size() is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("xx"));
            auto status = str.insert(2, other, 10, 5);

            THEN("operation succeeds and string is unchanged")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "example");
            }
        }

        WHEN("append(size_t count, T ch) is called")
        {
            auto status = str.append(3, '!');

            THEN("characters appended by count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "example!!!");
            }
        }

        WHEN("append(const basic_string&) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("bar"));
            auto status = str.append(other);

            THEN("basic_string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "examplebar");
            }
        }

        WHEN("append(const T* s) is called")
        {
            auto status = str.append("bar");

            THEN("entire C-string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "examplebar");
            }
        }

        WHEN("append(const T* s, size_t count) is called")
        {
            auto status = str.append("XYZ", 2);

            THEN("partial C-string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exampleXY");
            }
        }

        WHEN("append(const basic_string&, pos, count) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("other"));
            auto status = str.append(other, 0, 3);

            THEN("substring of basic_string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exampleoth");
            }
        }

        WHEN("append(const basic_string& str, size_type pos, size_type count) with pos > str.size() is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("XYZ"));
            auto status = str.append(other, 5, 2);

            THEN("operation succeeds and string is unchanged")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "example");
            }
        }

        WHEN("operator+=(const T*) is called")
        {
            auto status = (str += "Z");

            THEN("C-string concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exampleZ");
            }
        }

        WHEN("operator+=(T) is called")
        {
            auto status = (str += '!');

            THEN("single character concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "example!");
            }
        }

        WHEN("operator+=(const basic_string&) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("bar"));
            auto status = (str += other);

            THEN("basic_string concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "examplebar");
            }
        }

        WHEN("replace(size_t pos, size_t count, const basic_string&) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("AA"));
            auto status = str.replace(2, 3, other);

            THEN("substring replaced with basic_string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("AA") == 2);
                REQUIRE(str == "exAAle");
            }
        }

        WHEN("replace(size_type pos, size_type count, const T* s) is called")
        {
            auto status = str.replace(2, 3, "XYZ");

            THEN("substring replaced with C-string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exXYZle");
            }
        }

        WHEN("replace(size_type pos, size_type count, const T* s, size_type count2) is called")
        {
            auto status = str.replace(2, 2, "XX", 2);

            THEN("substring replaced with C-string and count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("XX") == 2);
                REQUIRE(str == "exXXple");
            }
        }

        WHEN("replace(size_type pos1, size_type count1, const basic_string& str, pos2, count2) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("12345"));
            auto status = str.replace(1, 3, other, 1, 2);

            THEN("substring replaced with substring of another string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "e23ple");
            }
        }

        WHEN("replace(size_type pos1, size_type count1, const basic_string& str, size_t pos2, size_t count2) with pos2 > other.size()) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("12"));
            auto status = str.replace(2, 3, other, 5, 7);

            THEN("range is erased")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exle");
            }
        }

        WHEN("replace(const_iterator first, const_iterator last, const basic_string& str) is called")
        {
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("ZZ"));
            auto status = str.replace(str.begin() + 2, str.begin() + 4, other);

            THEN("range replaced with another string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "exZZple");
            }
        }

        WHEN("replace(const_iterator first, const_iterator last, size_type count2, T ch) is called")
        {
            auto first = str.begin() + 2;
            auto last = first + 2;
            auto status = str.replace(first, last, 3, '*');

            THEN("replacement should be applied")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("***", 0) != decltype(str)::npos);
                REQUIRE(str == "ex***ple");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called")
        {
            auto first = str.begin() + 1;
            auto last = str.begin() + 3;
            auto after = str.erase(first, last);

            THEN("erase returns iterator to next element")
            {
                REQUIRE(*after == 'm');
                REQUIRE(str == "emple");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called with first == last")
        {
            auto it = str.erase(str.begin() + 2, str.begin() + 2);

            THEN("no characters erased, iterator points to first")
            {
                REQUIRE(it == str.begin() + 2);
                REQUIRE(str == "example");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called with first == end()")
        {
            auto it = str.erase(str.end(), str.end());

            THEN("no characters erased, iterator points to end")
            {
                REQUIRE(it == str.end());
                REQUIRE(str == "example");
            }
        }

        WHEN("erase(size_type index) with count = npos is called")
        {
            REQUIRE_NT_SUCCESS(str.erase(1));

            THEN("substring is erased")
            {
                REQUIRE(str == "e");
            }
        }

        WHEN("erase(size_type index, size_type count) is called")
        {
            REQUIRE_NT_SUCCESS(str.erase(1, 2));

            THEN("substring erased by index/count")
            {
                REQUIRE(str == "emple");
            }
        }

        WHEN("erase(size_type index, size_type count) is called with index >= size()")
        {
            THEN("erase should fail without crush")
            {
                REQUIRE_NT_SUCCESS(!str.erase(10, 2)); // index beyond size
            }
        }

        WHEN("erase(size_type index, size_type count) is called with count == 0")
        {
            REQUIRE_NT_SUCCESS(str.erase(2, 0));

            THEN("string remains unchanged")
            {
                REQUIRE(str == "example");
            }
        }

        WHEN("erase(size_type index, size_type count) is called with count > size() - index")
        {
            REQUIRE_NT_SUCCESS(str.erase(3, 10));

            THEN("erases until the end")
            {
                REQUIRE(str == "exa");
            }
        }

        WHEN("erase(const_iterator pos) is called")
        {
            auto it = str.erase(str.begin());

            THEN("first char erased")
            {
                REQUIRE(*it == 'x');
                REQUIRE(str == "xample");
            }
        }

        WHEN("erase(const_iterator pos) is called with pos == end()")
        {
            auto it = str.erase(str.end());

            THEN("no characters erased, iterator points to end")
            {
                REQUIRE(it == str.end());
                REQUIRE(str == "example");
            }
        }
    }
}

SCENARIO("basic_string: String operations")
{
    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("copy-me"));

        WHEN("copy(T* dest, size_type count, size_type pos) is called")
        {
            char buf[16] = {};
            auto n = str.copy(buf, 4, 5); // from index 5: 'm','e'

            THEN("buffer contains copied data and count is correct")
            {
                REQUIRE(n == 2);
                REQUIRE(buf[0] == 'm');
                REQUIRE(buf[1] == 'e');
            }
        }

        WHEN("copy(T* dest, size_type count, size_type pos) with pos == size is called")
        {
            char buf[8] = {};
            auto n = str.copy(buf, 4, str.size());

            THEN("nothing is copied and count is 0")
            {
                REQUIRE(n == 0);
                REQUIRE(buf[0] == '\0');
            }
        }

        WHEN("substr_copy(size_type pos, size_type count) is called")
        {
            auto sub = str.substr_copy(0, 4);

            THEN("substrings should be as expected")
            {
                REQUIRE(sub == "copy");
            }
        }

        WHEN("substr_copy(size_type pos, size_type count) with pos > size is called")
        {
            auto sub = str.substr_copy(100, 2); // out of range -> empty

            THEN("substrings should be empty")
            {
                REQUIRE(sub.empty());
            }
        }

        WHEN("substr_copy(size_type pos, size_type count) with count > available is called")
        {
            auto sub = str.substr_copy(3, 10);

            THEN("substring is truncated to available characters")
            {
                REQUIRE(sub == "y-me");
            }
        }

        WHEN("substr(size_type pos, size_type count) is called")
        {
            auto sub = str.substr(0, 4);

            THEN("substrings should be as expected")
            {
                REQUIRE(sub == "copy");
            }
        }

        WHEN("substr(size_type pos, size_type count) with pos > size is called")
        {
            auto sub = str.substr(100, 2); // out of range -> empty

            THEN("substrings should be empty")
            {
                REQUIRE(sub.empty());
            }
        }

        WHEN("substr(size_type pos, size_type count) with count > available is called")
        {
            auto sub = str.substr(3, 10);

            THEN("substring is truncated to available characters")
            {
                REQUIRE(sub == "y-me");
            }
        }
    }

    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("copy(T* dest, size_type count, size_type pos) is called")
        {
            char buf[8] = {};
            auto n = str.copy(buf, 4, 0);

            THEN("nothing is copied from an empty string")
            {
                REQUIRE(n == 0);
                REQUIRE(buf[0] == '\0');
            }
        }

        WHEN("substr_copy(size_type pos, size_type count) is called")
        {
            auto sub = str.substr_copy(0, 10);

            THEN("substr of empty is empty")
            {
                REQUIRE(sub.empty());
            }
        }

        WHEN("substr(size_type pos, size_type count) is called")
        {
            auto sub = str.substr(0, 10);

            THEN("substr of empty is empty")
            {
                REQUIRE(sub.empty());
            }
        }
    }
}

SCENARIO("basic_string: find/rfind and friends")
{
    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("abracadabra"));

        WHEN("find(const T* s, size_type pos) is called")
        {
            const auto posSub = str.find("abra");

            THEN("substring position should match")
            {
                REQUIRE(posSub == 0);
            }
        }

        WHEN("find(T ch, size_type pos) is called")
        {
            const auto posCh = str.find('c');

            THEN("character position should match")
            {
                REQUIRE(posCh == 4);
            }
        }

        WHEN("rfind(const T* s, size_type pos) is called")
        {
            const auto posRSub = str.rfind("abra");

            THEN("reverse substring position should match")
            {
                REQUIRE(posRSub == 7);
            }
        }

        WHEN("rfind(T ch, size_type pos) is called")
        {
            const auto posRCh = str.rfind('a');

            THEN("reverse character position should match")
            {
                REQUIRE(posRCh == 10);
            }
        }

        WHEN("find_first_of(const T* s, size_type pos) is called")
        {
            const auto posFirst = str.find_first_of("xyzabc");

            THEN("it finds any of the given set from the beginning")
            {
                REQUIRE(posFirst == 0);
            }
        }

        WHEN("find_last_of(const T* s, size_type pos) is called")
        {
            const auto posLast = str.find_last_of("xyzabc");

            THEN("it finds any of the given set from the end")
            {
                REQUIRE(posLast == 10);
            }
        }

        WHEN("find_first_not_of(const T* s, size_type pos) is called")
        {
            const auto posFirstNot = str.find_first_not_of("ab");

            THEN("it skips the matching set from the beginning")
            {
                REQUIRE(posFirstNot == 2); // 'r'
            }
        }

        WHEN("find_last_not_of(const T* s, size_type pos) is called")
        {
            const auto posLastNot = str.find_last_not_of("ar");

            THEN("it skips the matching set from the end")
            {
                REQUIRE(posLastNot == 8);  // before 'b'
            }
        }

        WHEN("find(const T* s, size_type pos) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            REQUIRE_NT_SUCCESS(wstr.assign(L"Hello Wide World"));
            const auto posWide = wstr.find(L"Wide");

            THEN("wide substring search works")
            {
                REQUIRE(posWide == 6);
            }
        }

        WHEN("find(T ch, size_type pos) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            REQUIRE_NT_SUCCESS(wstr.assign(L"Hello Wide World"));
            const auto posWCh = wstr.find(L'W');

            THEN("wide character search works")
            {
                REQUIRE(posWCh == 6);
            }
        }

        WHEN("find_first_of(const T* s, size_type pos) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            REQUIRE_NT_SUCCESS(wstr.assign(L"Hello Wide World"));
            const auto posFirst = wstr.find_first_of(L"HW");

            THEN("wide find_first_of works")
            {
                REQUIRE(posFirst == 0);
            }
        }

        WHEN("find_last_of(const T* s, size_type pos) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> wstr;
            REQUIRE_NT_SUCCESS(wstr.assign(L"Hello Wide World"));
            const auto posLast = wstr.find_last_of(L"od");

            THEN("wide find_last_of works")
            {
                REQUIRE(posLast == 15);
            }
        }
    }

    GIVEN("an empty basic_string")
    {
        kf::basic_string<char, PagedPool> str;

        WHEN("find(const T* s, size_type pos) is called")
        {
            THEN("find on empty returns npos")
            {
                REQUIRE(str.find("a") == decltype(str)::npos);
            }
        }

        WHEN("find(T ch, size_type pos) is called")
        {
            THEN("find on empty returns npos")
            {
                REQUIRE(str.find('a') == decltype(str)::npos);
            }
        }

        WHEN("rfind(const T* s, size_type pos) is called")
        {
            THEN("rfind on empty returns npos")
            {
                REQUIRE(str.rfind("a") == decltype(str)::npos);
            }
        }

        WHEN("rfind(T ch, size_type pos) is called")
        {
            THEN("rfind on empty returns npos")
            {
                REQUIRE(str.rfind('a') == decltype(str)::npos);
            }
        }

        WHEN("find_first_of(const T* s, size_type pos) is called")
        {
            THEN("find_first_of on empty returns npos")
            {
                REQUIRE(str.find_first_of("abc") == decltype(str)::npos);
            }
        }

        WHEN("find_last_of(const T* s, size_type pos) is called")
        {
            THEN("find_last_of on empty returns npos")
            {
                REQUIRE(str.find_last_of("abc") == decltype(str)::npos);
            }
        }

        WHEN("find_first_not_of(const T* s, size_type pos) is called")
        {
            THEN("find_first_not_of on empty returns npos")
            {
                REQUIRE(str.find_first_not_of("abc") == decltype(str)::npos);
            }
        }

        WHEN("find_last_not_of(const T* s, size_type pos) is called")
        {
            THEN("find_last_not_of on empty returns npos")
            {
                REQUIRE(str.find_last_not_of("abc") == decltype(str)::npos);
            }
        }
    }
}