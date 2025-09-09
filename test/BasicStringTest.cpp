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

        WHEN("assign(const T* s) is called with empty C-string")
        {
            auto status = str.assign("");

            THEN("the string remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.empty());
                REQUIRE(str.size() == 0);
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
            REQUIRE(str[0] == 'a');
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
            THEN("data should not be null")
            {
                REQUIRE(str.data() != nullptr);
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
            REQUIRE_NT_SUCCESS(wstr.assign(L"xyz"));
            auto refOpt = wstr.at(0);

            THEN("wide element should be accessible")
            {
                REQUIRE(refOpt.has_value());
                REQUIRE(refOpt->get() == L'x');
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
            std::array<char, 8> acc{};
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
            std::array<char, 8> acc{};
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

        WHEN("cbegin() is called on a const basic_string")
        {
            const auto& cstr = str;
            auto it = cstr.cbegin();

            THEN("dereferencing works")
            {
                REQUIRE(*it == 'a');
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

        WHEN("shrink_to_fit() is called")
        {
            // shrink_to_fit has no effect on small strings stored in SSO (size <= 15 chars);
            REQUIRE_NT_SUCCESS(str.assign("1-2-3-4-5-6-7-8-9-10-11-12-13-14"));
            auto status = str.shrink_to_fit();
            const auto capNow = str.capacity();
            const auto sizeNow = str.size();

            THEN("capacity should be equal to expected")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(capNow == sizeNow);
            }
        }

        WHEN("length() is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("xx"));
            const auto len = str.length();
            const auto sz = str.size();

            THEN("length equals size")
            {
                REQUIRE(len == sz);
            }
        }

        WHEN("max_size() is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("x"));
            const auto max = str.max_size();
            const auto sz = str.size();

            THEN("max_size is sane")
            {
                REQUIRE(max >= sz);
            }
        }

        WHEN("resize(size_type count, const T& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("hi"));
            auto status = str.resize(5, 'x');

            THEN("the string should be grown with fill value")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "hixxx");
            }
        }

        WHEN("resize(size_type count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("hi"));
            auto status = str.resize(2);

            THEN("the string should be shrunk")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "hi");
            }
        }
    }

    GIVEN("a basic_string with content")
    {
        kf::basic_string<char, PagedPool> str;
        REQUIRE_NT_SUCCESS(str.assign("abcdef"));

        WHEN("size() is called")
        {
            THEN("size equals number of characters")
            {
                REQUIRE(str.size() == 6);
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
            REQUIRE_NT_SUCCESS(str.assign("1-2-3-4-5-6-7-8-9"));
            REQUIRE_NT_SUCCESS(str.reserve(64));
            auto status = str.shrink_to_fit();
            const auto capNow = str.capacity();
            const auto sizeNow = str.size();

            THEN("capacity equals expected capacity and content preserved")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(capNow == sizeNow);
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

        WHEN("length() is called")
        {
            THEN("length equals size")
            {
                REQUIRE(str.length() == str.size());
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
    GIVEN("two basic_strings with content")
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
                REQUIRE(cmp == 0); // -1
            }
        }

        WHEN("operator==(const T*) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> w;
            REQUIRE_NT_SUCCESS(w.assign(L"zeta"));

            THEN("wide equality works with wide literals")
            {
                REQUIRE(w == L"zeta");
            }
        }

        WHEN("operator!=(const T*) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> w;
            REQUIRE_NT_SUCCESS(w.assign(L"zeta"));

            THEN("wide inequality works with wide literals")
            {
                REQUIRE(w != L"eta");
            }
        }

        WHEN("operator<(const T*) is called on a wide string")
        {
            kf::basic_string<wchar_t, PagedPool> w;
            REQUIRE_NT_SUCCESS(w.assign(L"zeta"));

            THEN("wide less-than works with wide literals")
            {
                REQUIRE(w < L"zzzz");
            }
        }
    }

    GIVEN("one of basic_strings with content")
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
            REQUIRE_NT_SUCCESS(str.assign("core"));
            str.clear();

            THEN("string is empty")
            {
                REQUIRE(str.empty());
                REQUIRE(str.size() == 0);
            }
        }

        WHEN("push_back(const T& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abc"));
            const auto ch = 'd';
            auto status = str.push_back(ch);

            THEN("element is appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abcd");
            }
        }

        WHEN("push_back(T&& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abc"));
            char ch = 'd';
            auto status = str.push_back(std::move(ch));

            THEN("element is appended using rvalue reference")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abcd");
            }
        }

        WHEN("pop_back() is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcd"));
            str.pop_back();

            THEN("last char removed")
            {
                REQUIRE(str == "abc");
            }
        }

        WHEN("insert(const_iterator pos, const T& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("ABCD"));
            auto itOpt = str.insert(str.begin() + 2, 'x'); // ABxCD

            THEN("iterator is returned and string modified")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "ABxCD");
            }
        }

        WHEN("insert(const_iterator pos, T&& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("ABCD"));
            char ch = 'x';
            auto itOpt = str.insert(str.begin() + 2, std::move(ch)); // ABxCD

            THEN("iterator is returned and rvalue inserted")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "ABxCD");
            }
        }

        WHEN("insert(const_iterator pos, size_type count, const T& value) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("ABCD"));
            auto itOpt = str.insert(str.begin() + 2, 3, 'y'); // AByyyCD

            THEN("multiple copies inserted at iterator position")
            {
                REQUIRE(itOpt.has_value());
                REQUIRE(str == "AByyyCD");
            }
        }

        WHEN("insert(size_type index, const T* s, size_type count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abc"));
            auto status = str.insert(1, "XYZ", 2); // aXYbc

            THEN("substring of C-string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "aXYbc");
            }
        }

        WHEN("insert(size_type index, size_type count, T ch) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abc"));
            auto status = str.insert(1, 2, 'Y'); // aYYbc

            THEN("characters inserted by count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "aYYbc");
            }
        }

        WHEN("insert(size_type index, const T* s) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("aYYbc"));
            auto status = str.insert(0, "--"); // --aYYbc

            THEN("C-string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "--aYYbc");
            }
        }

        WHEN("insert(size_type index, const basic_string&) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("--aYYbc"));
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("[]"));
            auto status = str.insert(2, other); // --[]aYYbc

            THEN("basic_string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "--[]aYYbc");
            }
        }

        WHEN("insert(size_type index, const basic_string&, size_type index_str, size_type count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("--[]aYYbc"));
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("[]"));
            auto status = str.insert(2, other, 1, 1); // --] []aYYbc -> --][]aYYbc

            THEN("substring from basic_string inserted")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "--][]aYYbc");
            }
        }

        WHEN("insert(size_type index, const basic_string&, size_type index_str, size_type count) with index_str > str.size() is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("base"));
            kf::basic_string<char, PagedPool> src;
            REQUIRE_NT_SUCCESS(src.assign("xx"));
            auto status = str.insert(2, src, 10, 5);

            THEN("operation succeeds and string is unchanged")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "base");
            }
        }

        WHEN("append(size_t count, T ch) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo"));
            auto status = str.append(3, '!');

            THEN("characters appended by count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foo!!!");
            }
        }

        WHEN("append(const basic_string&) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo!!!"));
            kf::basic_string<char, PagedPool> more;
            REQUIRE_NT_SUCCESS(more.assign("bar"));
            auto status = str.append(more);

            THEN("basic_string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foo!!!bar");
            }
        }

        WHEN("append(const T* s) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo"));
            auto status = str.append("bar");

            THEN("entire C-string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foobar");
            }
        }

        WHEN("append(const T* s, size_t count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo!!!bar"));
            auto status = str.append("XYZ", 2);

            THEN("partial C-string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foo!!!barXY");
            }
        }

        WHEN("append(const basic_string&, pos, count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("base"));
            kf::basic_string<char, PagedPool> more;
            REQUIRE_NT_SUCCESS(more.assign("appendix"));
            auto status = str.append(more, 0, 3); // "baseapp"

            THEN("substring of basic_string appended")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "baseapp");
            }
        }

        WHEN("append(const basic_string& str, size_type pos, size_type count) with pos > str.size() is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("A"));
            kf::basic_string<char, PagedPool> src;
            REQUIRE_NT_SUCCESS(src.assign("XYZ"));
            auto status = str.append(src, 5, 2);

            THEN("operation succeeds and string is unchanged")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "A");
            }
        }

        WHEN("operator+=(const T*) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo!!!barXY"));
            auto status = (str += "Z");

            THEN("C-string concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foo!!!barXYZ");
            }
        }

        WHEN("operator+=(T) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo!!!barXYZ"));
            auto status = (str += '!');

            THEN("single character concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foo!!!barXYZ!");
            }
        }

        WHEN("operator+=(const basic_string&) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("foo"));
            kf::basic_string<char, PagedPool> more;
            REQUIRE_NT_SUCCESS(more.assign("bar"));
            auto status = (str += more);

            THEN("basic_string concatenated")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "foobar");
            }
        }

        WHEN("replace(size_t pos, size_t count, const basic_string&) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("0123456789"));
            kf::basic_string<char, PagedPool> tmp;
            REQUIRE_NT_SUCCESS(tmp.assign("AA"));
            auto status = str.replace(2, 3, tmp); // 01 + AA + 56789

            THEN("substring replaced with basic_string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("AA") == 2);
            }
        }

        WHEN("replace(size_type pos, size_type count, const T* s) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcdef"));
            auto status = str.replace(2, 3, "XYZ"); // abXYZf

            THEN("substring replaced with C-string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abXYZf");
            }
        }

        WHEN("replace(size_type pos, size_type count, const T* s, size_type count2) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("01AA56789"));
            auto status = str.replace(2, 2, "XX", 2);

            THEN("substring replaced with C-string and count")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("XX") == 2);
            }
        }

        WHEN("replace(size_type pos1, size_type count1, const basic_string& str, pos2, count2) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcdef"));
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("12345"));
            auto status = str.replace(1, 3, other, 1, 2); // a + "23" + ef

            THEN("substring replaced with substring of another string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "a23ef");
            }
        }

        WHEN("replace(size_type pos1, size_type count1, const basic_string& str, size_t pos2, size_t count2) with pos2 > other.size()) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcdef"));
            kf::basic_string<char, PagedPool> other;
            REQUIRE_NT_SUCCESS(other.assign("12"));
            auto status = str.replace(2, 3, other, 5, 7);

            THEN("range is erased")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abf");
            }
        }

        WHEN("replace(const_iterator first, const_iterator last, const basic_string& str) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcdef"));
            kf::basic_string<char, PagedPool> repl;
            REQUIRE_NT_SUCCESS(repl.assign("ZZ"));
            auto status = str.replace(str.begin() + 2, str.begin() + 4, repl); // abZZef

            THEN("range replaced with another string")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str == "abZZef");
            }
        }

        WHEN("replace(const_iterator first, const_iterator last, size_type count2, T ch) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("--XX56789"));
            auto first = str.begin() + 2;
            auto last  = first + 2;
            auto status = str.replace(first, last, 3, '*');

            THEN("replacement should be applied")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(str.find("***", 0) != decltype(str)::npos);
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("ABxCD"));
            auto first = str.begin() + 1; // B
            auto last = str.begin() + 3; // x
            auto after = str.erase(first, last); // remove Bx -> ACD

            THEN("erase returns iterator to next element")
            {
                REQUIRE(*after == 'C');
                REQUIRE(str == "ACD");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called with first == last")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcde"));
            auto it = str.erase(str.begin() + 2, str.begin() + 2);

            THEN("no characters erased, iterator points to first")
            {
                REQUIRE(it == str.begin() + 2);
                REQUIRE(str == "abcde");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called with first == end()")
        {
            REQUIRE_NT_SUCCESS(str.assign("abcde"));
            auto it = str.erase(str.end(), str.end());

            THEN("no characters erased, iterator points to end")
            {
                REQUIRE(it == str.end());
                REQUIRE(str == "abcde");
            }
        }

        WHEN("erase(const_iterator first, const_iterator last) is called on empty string")
        {
            auto it = str.erase(str.begin(), str.end());

            THEN("iterator points to begin/end")
            {
                REQUIRE(it == str.begin());
                REQUIRE(str.empty());
            }
        }

        WHEN("erase(size_type index, size_type count) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("erase"));
            REQUIRE_NT_SUCCESS(str.erase(1, 2)); // e + se -> ese

            THEN("substring erased by index/count")
            {
                REQUIRE(str == "ese");
            }
        }

        WHEN("erase(size_type index, size_type count) is called with index >= size()")
        {
            REQUIRE_NT_SUCCESS(str.assign("hello"));

            THEN("erase should fail without crush")
            {
                REQUIRE_NT_SUCCESS(!str.erase(10, 2)); // index beyond size
            }
        }

        WHEN("erase(size_type index, size_type count) is called with count == 0")
        {
            REQUIRE_NT_SUCCESS(str.assign("hello"));
            REQUIRE_NT_SUCCESS(str.erase(2, 0));

            THEN("string remains unchanged")
            {
                REQUIRE(str == "hello");
            }
        }

        WHEN("erase(size_type index, size_type count) is called with count > size() - index")
        {
            REQUIRE_NT_SUCCESS(str.assign("hello"));
            REQUIRE_NT_SUCCESS(str.erase(3, 10));

            THEN("erases until the end")
            {
                REQUIRE(str == "hel");
            }
        }

        WHEN("erase(const_iterator pos) is called")
        {
            REQUIRE_NT_SUCCESS(str.assign("ese"));
            auto it = str.erase(str.begin());

            THEN("first char erased")
            {
                REQUIRE(*it == 's');
                REQUIRE(str == "se");
            }
        }

        WHEN("erase(const_iterator pos) is called with pos == end()")
        {
            REQUIRE_NT_SUCCESS(str.assign("abc"));
            auto it = str.erase(str.end());

            THEN("no characters erased, iterator points to end")
            {
                REQUIRE(it == str.end());
                REQUIRE(str == "abc");
            }
        }

        WHEN("erase(const_iterator pos) is called on empty string")
        {
            str.clear();
            auto it = str.erase(str.begin());

            THEN("no characters erased, iterator points to begin")
            {
                REQUIRE(it == str.begin());
                REQUIRE(str.empty());
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