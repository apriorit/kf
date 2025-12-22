#include "pch.h"
#include <kf/stl/vector>

SCENARIO("vector size and capacity")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("vector size is 0, capacity is 0")
            {
                REQUIRE(v.size() == 0);
                REQUIRE(v.capacity() == 0);
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("vector size is 1, capacity is >= 1")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() >= 1);
            }
        }

        WHEN("assign 1000 elements with value 12345")
        {
            REQUIRE_NT_SUCCESS(v.assign(1000, 12345));

            THEN("vector size is 1000, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1000);
                REQUIRE(v.capacity() >= 1000);
            }
        }

        WHEN("resize to 1000")
        {
            REQUIRE_NT_SUCCESS(v.resize(1000));

            THEN("vector size is 1000, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1000);
                REQUIRE(v.capacity() >= 1000);
            }
        }
    }

    GIVEN("vector with 1000 elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.assign(1000, 12345));

        WHEN("resize to 1")
        {
            REQUIRE_NT_SUCCESS(v.resize(1));

            THEN("vector size is 1, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() >= 1000);
            }
        }

        WHEN("resize to 1 and shrink_to_fit")
        {
            REQUIRE_NT_SUCCESS(v.resize(1));
            REQUIRE_NT_SUCCESS(v.shrink_to_fit());

            THEN("vector size is 1, capacity is 1")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() == 1);
            }
        }

        WHEN("clear")
        {
            v.clear();

            THEN("vector size is 0, capacity is >= 1000")
            {
                REQUIRE(v.size() == 0);
                REQUIRE(v.capacity() >= 1000);
            }
        }
    }
}

SCENARIO("vector iterators")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("(c)(r)begin == (c)(r)end")
            {
                REQUIRE(v.begin() == v.end());
                REQUIRE(v.cbegin() == v.cend());
                REQUIRE(v.rbegin() == v.rend());
                REQUIRE(v.crbegin() == v.crend());
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("difference between (c)(r)begin and (c)(r)end is 1")
            {
                REQUIRE(v.end() - v.begin() == 1);
                REQUIRE(v.cend() - v.cbegin() == 1);
                REQUIRE(v.rend() - v.rbegin() == 1);
                REQUIRE(v.crend() - v.crbegin() == 1);
            }
        }
    }
}

SCENARIO("vector at and [] access")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("at returns nullopt")
            {
                REQUIRE(v.at(0) == std::nullopt);
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("value of element with index 0 is 100")
            {
                REQUIRE(v[0] == 100);
                REQUIRE(v.at(0).has_value());
                REQUIRE(*v.at(0) == 100);
            }
        }
    }
}

SCENARIO("vector move constructor and move assignment")
{
    GIVEN("vector with 3 elements")
    {
        kf::vector<int, PagedPool> v1;
        REQUIRE_NT_SUCCESS(v1.push_back(10));
        REQUIRE_NT_SUCCESS(v1.push_back(20));
        REQUIRE_NT_SUCCESS(v1.push_back(30));

        WHEN("move construct new vector")
        {
            kf::vector<int, PagedPool> v2(std::move(v1));

            THEN("new vector contains the elements")
            {
                REQUIRE(v2.size() == 3);
                REQUIRE(v2[0] == 10);
                REQUIRE(v2[1] == 20);
                REQUIRE(v2[2] == 30);
            }
        }

        WHEN("move assign to new vector")
        {
            kf::vector<int, PagedPool> v2;
            v2 = std::move(v1);

            THEN("new vector contains the elements")
            {
                REQUIRE(v2.size() == 3);
                REQUIRE(v2[0] == 10);
                REQUIRE(v2[1] == 20);
                REQUIRE(v2[2] == 30);
            }
        }
    }
}

SCENARIO("vector front, back and data access")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("push_back single element")
        {
            REQUIRE_NT_SUCCESS(v.push_back(42));

            THEN("front and back return the same element")
            {
                REQUIRE(v.front() == 42);
                REQUIRE(v.back() == 42);
                REQUIRE(v.data() != nullptr);
                REQUIRE(*v.data() == 42);
            }
        }
    }

    GIVEN("vector with multiple elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(2));
        REQUIRE_NT_SUCCESS(v.push_back(3));

        WHEN("do nothing")
        {
            THEN("front returns first element, back returns last element")
            {
                REQUIRE(v.front() == 1);
                REQUIRE(v.back() == 3);
                REQUIRE(v.data() != nullptr);
                REQUIRE(v.data()[0] == 1);
                REQUIRE(v.data()[1] == 2);
                REQUIRE(v.data()[2] == 3);
            }
        }

        WHEN("modify front and back")
        {
            v.front() = 10;
            v.back() = 30;

            THEN("elements are modified")
            {
                REQUIRE(v[0] == 10);
                REQUIRE(v[2] == 30);
                REQUIRE(v[1] == 2); // middle element unchanged
            }
        }
    }
}

SCENARIO("vector capacity operations")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("empty returns true, max_size > 0")
            {
                REQUIRE(v.empty() == true);
                REQUIRE(v.max_size() > 0);
            }
        }

        WHEN("reserve 100 elements")
        {
            REQUIRE_NT_SUCCESS(v.reserve(100));

            THEN("capacity is at least 100, size is still 0")
            {
                REQUIRE(v.capacity() >= 100);
                REQUIRE(v.size() == 0);
                REQUIRE(v.empty() == true);
            }
        }

        WHEN("push_back element after reserve")
        {
            REQUIRE_NT_SUCCESS(v.reserve(100));
            REQUIRE_NT_SUCCESS(v.push_back(123));

            THEN("empty returns false")
            {
                REQUIRE(v.empty() == false);
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 123);
            }
        }
    }

    GIVEN("vector with elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(2));

        WHEN("reserve smaller capacity")
        {
            auto original_capacity = v.capacity();
            REQUIRE_NT_SUCCESS(v.reserve(1));

            THEN("capacity is not reduced")
            {
                REQUIRE(v.capacity() == original_capacity);
                REQUIRE(v.size() == 2);
            }
        }
    }
}

SCENARIO("vector get_allocator")
{
    GIVEN("vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("get allocator")
        {
            auto alloc = v.get_allocator();

            THEN("allocator is returned successfully")
            {
                // Just verify we can call the method
                // The allocator type is kf::Allocator<int, PagedPool>
                static_assert(std::is_same_v<decltype(alloc), kf::Allocator<int, PagedPool>>);
            }
        }
    }
}

SCENARIO("vector insert operations")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("insert single element at begin")
        {
            auto iter = v.insert(v.begin(), 100);

            THEN("element is inserted successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 100);
            }
        }

        WHEN("insert move element at begin")
        {
            int value = 200;
            auto iter = v.insert(v.begin(), std::move(value));

            THEN("element is inserted successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 200);
            }
        }

        WHEN("insert multiple copies")
        {
            auto iter = v.insert(v.begin(), 3, 50);

            THEN("elements are inserted successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 50);
                REQUIRE(v[1] == 50);
                REQUIRE(v[2] == 50);
            }
        }
    }

    GIVEN("vector with elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(3));

        WHEN("insert in middle")
        {
            auto iter = v.insert(v.begin() + 1, 2);

            THEN("element is inserted in correct position")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 2);
                REQUIRE(v[2] == 3);
            }
        }
    }
}

SCENARIO("vector insert range operations")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("insert range from array at begin")
        {
            int arr[] = {10, 20, 30};
            auto iter = v.insert(v.begin(), std::begin(arr), std::end(arr));

            THEN("elements are inserted successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 10);
                REQUIRE(v[1] == 20);
                REQUIRE(v[2] == 30);
            }
        }

        WHEN("insert range from another vector")
        {
            kf::vector<int, PagedPool> source;
            REQUIRE_NT_SUCCESS(source.push_back(100));
            REQUIRE_NT_SUCCESS(source.push_back(200));
            REQUIRE_NT_SUCCESS(source.push_back(300));

            auto iter = v.insert(v.begin(), source.begin(), source.end());

            THEN("elements are inserted successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 100);
                REQUIRE(v[1] == 200);
                REQUIRE(v[2] == 300);
            }
        }

        WHEN("insert empty range")
        {
            int arr[] = {1, 2, 3};
            auto iter = v.insert(v.begin(), std::begin(arr), std::begin(arr));

            THEN("nothing is inserted")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 0);
                REQUIRE(v.empty() == true);
            }
        }
    }

    GIVEN("vector with elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(5));

        WHEN("insert range at beginning")
        {
            int arr[] = {-2, -1, 0};
            auto iter = v.insert(v.begin(), std::begin(arr), std::end(arr));

            THEN("elements are inserted at correct position")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 5);
                REQUIRE(v[0] == -2);
                REQUIRE(v[1] == -1);
                REQUIRE(v[2] == 0);
                REQUIRE(v[3] == 1);
                REQUIRE(v[4] == 5);
            }
        }

        WHEN("insert range in middle")
        {
            int arr[] = {2, 3, 4};
            auto iter = v.insert(v.begin() + 1, std::begin(arr), std::end(arr));

            THEN("elements are inserted in correct position")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 5);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 2);
                REQUIRE(v[2] == 3);
                REQUIRE(v[3] == 4);
                REQUIRE(v[4] == 5);
            }
        }

        WHEN("insert range at end")
        {
            int arr[] = {6, 7, 8};
            auto iter = v.insert(v.end(), std::begin(arr), std::end(arr));

            THEN("elements are inserted at end")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 5);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 5);
                REQUIRE(v[2] == 6);
                REQUIRE(v[3] == 7);
                REQUIRE(v[4] == 8);
            }
        }
    }
}

SCENARIO("vector emplace operations")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("emplace single element")
        {
            auto iter = v.emplace(v.begin(), 100);

            THEN("element is emplaced successfully")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 100);
            }
        }

        WHEN("emplace_back element")
        {
            auto ref = v.emplace_back(200);

            THEN("element is emplaced successfully")
            {
                REQUIRE(ref.has_value());
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 200);
                REQUIRE(ref->get() == 200);
            }
        }
    }

    GIVEN("vector with elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(3));

        WHEN("emplace in middle")
        {
            auto iter = v.emplace(v.begin() + 1, 2);

            THEN("element is emplaced in correct position")
            {
                REQUIRE(iter.has_value());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 2);
                REQUIRE(v[2] == 3);
            }
        }

        WHEN("emplace_back multiple elements")
        {
            auto ref1 = v.emplace_back(4);
            auto ref2 = v.emplace_back(5);

            THEN("elements are emplaced successfully")
            {
                REQUIRE(ref1.has_value());
                REQUIRE(ref2.has_value());
                REQUIRE(v.size() == 4);
                REQUIRE(v[2] == 4);
                REQUIRE(v[3] == 5);
            }
        }
    }
}

SCENARIO("vector erase operations")
{
    GIVEN("vector with single element")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(100));

        WHEN("erase single element")
        {
            auto iter = v.erase(v.begin());

            THEN("vector becomes empty")
            {
                REQUIRE(iter == v.end());
                REQUIRE(v.size() == 0);
                REQUIRE(v.empty() == true);
            }
        }
    }

    GIVEN("vector with multiple elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(2));
        REQUIRE_NT_SUCCESS(v.push_back(3));
        REQUIRE_NT_SUCCESS(v.push_back(4));

        WHEN("erase first element")
        {
            auto iter = v.erase(v.begin());

            THEN("first element is removed")
            {
                REQUIRE(iter == v.begin());
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 2);
                REQUIRE(v[1] == 3);
                REQUIRE(v[2] == 4);
            }
        }

        WHEN("erase middle element")
        {
            auto iter = v.erase(v.begin() + 1);

            THEN("middle element is removed")
            {
                REQUIRE(v.size() == 3);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 3);
                REQUIRE(v[2] == 4);
            }
        }

        WHEN("erase range of elements")
        {
            auto iter = v.erase(v.begin() + 1, v.begin() + 3);

            THEN("range is removed")
            {
                REQUIRE(v.size() == 2);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 4);
            }
        }
    }
}

SCENARIO("vector pop_back operation")
{
    GIVEN("vector with single element")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(100));

        WHEN("pop_back")
        {
            v.pop_back();

            THEN("vector becomes empty")
            {
                REQUIRE(v.size() == 0);
                REQUIRE(v.empty() == true);
            }
        }
    }

    GIVEN("vector with multiple elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(1));
        REQUIRE_NT_SUCCESS(v.push_back(2));
        REQUIRE_NT_SUCCESS(v.push_back(3));

        WHEN("pop_back")
        {
            v.pop_back();

            THEN("last element is removed")
            {
                REQUIRE(v.size() == 2);
                REQUIRE(v[0] == 1);
                REQUIRE(v[1] == 2);
            }
        }

        WHEN("pop_back multiple times")
        {
            v.pop_back();
            v.pop_back();

            THEN("multiple elements are removed")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v[0] == 1);
            }
        }
    }
}

SCENARIO("vector swap operation")
{
    GIVEN("two vectors with different contents")
    {
        kf::vector<int, PagedPool> v1;
        kf::vector<int, PagedPool> v2;

        REQUIRE_NT_SUCCESS(v1.push_back(1));
        REQUIRE_NT_SUCCESS(v1.push_back(2));

        REQUIRE_NT_SUCCESS(v2.push_back(10));
        REQUIRE_NT_SUCCESS(v2.push_back(20));
        REQUIRE_NT_SUCCESS(v2.push_back(30));

        WHEN("swap vectors")
        {
            v1.swap(v2);

            THEN("contents are swapped")
            {
                REQUIRE(v1.size() == 3);
                REQUIRE(v1[0] == 10);
                REQUIRE(v1[1] == 20);
                REQUIRE(v1[2] == 30);

                REQUIRE(v2.size() == 2);
                REQUIRE(v2[0] == 1);
                REQUIRE(v2[1] == 2);
            }
        }
    }

    GIVEN("one empty vector and one with elements")
    {
        kf::vector<int, PagedPool> v1;
        kf::vector<int, PagedPool> v2;

        REQUIRE_NT_SUCCESS(v2.push_back(100));
        REQUIRE_NT_SUCCESS(v2.push_back(200));

        WHEN("swap vectors")
        {
            v1.swap(v2);

            THEN("empty vector gets elements, other becomes empty")
            {
                REQUIRE(v1.size() == 2);
                REQUIRE(v1[0] == 100);
                REQUIRE(v1[1] == 200);

                REQUIRE(v2.size() == 0);
                REQUIRE(v2.empty() == true);
            }
        }
    }
}

SCENARIO("vector comprehensive iterator testing")
{
    GIVEN("vector with multiple elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.push_back(10));
        REQUIRE_NT_SUCCESS(v.push_back(20));
        REQUIRE_NT_SUCCESS(v.push_back(30));
        REQUIRE_NT_SUCCESS(v.push_back(40));

        WHEN("iterate forward with begin/end")
        {
            int sum = 0;
            for (auto it = v.begin(); it != v.end(); ++it)
            {
                sum += *it;
            }

            THEN("sum is correct")
            {
                REQUIRE(sum == 100);
            }
        }

        WHEN("iterate forward with cbegin/cend")
        {
            int sum = 0;
            for (auto it = v.cbegin(); it != v.cend(); ++it)
            {
                sum += *it;
            }

            THEN("sum is correct")
            {
                REQUIRE(sum == 100);
            }
        }

        WHEN("iterate backward with rbegin/rend")
        {
            int first_value = *v.rbegin();

            THEN("first value from reverse iterator is last element")
            {
                REQUIRE(first_value == 40);
            }
        }

        WHEN("iterate backward with crbegin/crend")
        {
            int sum = 0;
            for (auto it = v.crbegin(); it != v.crend(); ++it)
            {
                sum += *it;
            }

            THEN("sum is correct")
            {
                REQUIRE(sum == 100);
            }
        }

        WHEN("modify through non-const iterators")
        {
            for (auto it = v.begin(); it != v.end(); ++it)
            {
                *it += 1;
            }

            THEN("all elements are incremented")
            {
                REQUIRE(v[0] == 11);
                REQUIRE(v[1] == 21);
                REQUIRE(v[2] == 31);
                REQUIRE(v[3] == 41);
            }
        }

        WHEN("use range-based for loop")
        {
            int count = 0;
            for (const auto& element : v)
            {
                count++;
                REQUIRE(element >= 10);
                REQUIRE(element <= 40);
            }

            THEN("all elements are visited")
            {
                REQUIRE(count == 4);
            }
        }
    }
}