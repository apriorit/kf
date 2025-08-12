#include "pch.h"
#include <kf/patterns/Observer.h>
#include <string_view>

namespace
{
    template<typename... T>
    class TestObserver : public kf::IObserver<T...>
    {
    public:
        void onNotify(T... t) override
        {
            tpl = std::make_tuple(t...);
            m_isNotified = true;
        }
        std::tuple<T...> tpl;
        bool m_isNotified = false;
    };

    class TestRefObserver : public kf::IObserver<int&, std::array<wchar_t, 4>&>
    {
    public:
        void onNotify(int& value, std::array<wchar_t, 4>& arr) override
        {
            value = 456;
            arr = {L'D', L'o', L'n', L'e'};
            m_isNotified = true;
        }

        bool m_isNotified = false;
    };

    template<typename... T>
    class TestMonoObservable : public kf::MonoObservable<T...>
    {
    public:
        void notify(T... t)
        {
            kf::MonoObservable<T...>::notify(t...);
        }
    };
}

SCENARIO("Testing MonoObservable functionality")
{
    GIVEN("A TestMonoObservable and a TestObserver with 2 types")
    {
        TestMonoObservable<int, std::wstring_view> observable;
        TestObserver<int, std::wstring_view> observer;

        WHEN("An observer is notified")
        {
            int value = 123;
            std::wstring_view message(L"Test");

            observable.attach(&observer);
            observable.notify(value, message);

            THEN("The observer is attached and receives the notification")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(std::get<0>(observer.tpl) == value);
                REQUIRE(std::get<1>(observer.tpl) == message);
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, std::wstring_view> observable;
        TestObserver<int, std::wstring_view> observer;

        WHEN("An observer is detached")
        {
            int value = 123;
            std::wstring_view message(L"Test");
            observable.attach(&observer);
            observable.detach(&observer);
            observable.notify(value, message);

            THEN("The observer does not receive notifications")
            {
                REQUIRE(observer.m_isNotified == false);
            }

            THEN("Second detach should do nothing")
            {
                observable.detach(&observer);
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver with 3 types")
    {
        TestMonoObservable<int, std::wstring_view, int> observable;
        TestObserver<int, std::wstring_view, int> observer;

        WHEN("Multiple notifications are sent")
        {
            int firstA = 123;
            int firstB = 456;
            std::wstring_view firstMessage(L"First");
            observable.attach(&observer);
            observable.notify(firstA, firstMessage, firstB);
            THEN("First notification is received")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(std::get<0>(observer.tpl) == firstA);
                REQUIRE(std::get<1>(observer.tpl) == firstMessage);
                REQUIRE(std::get<2>(observer.tpl) == firstB);
            }

            int secondA = 567;
            int secondB = 890;
            std::wstring_view secondMessage(L"Second");
            observable.notify(secondA, secondMessage, secondB);

            THEN("Second notification is received")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(std::get<0>(observer.tpl) == secondA);
                REQUIRE(std::get<1>(observer.tpl) == secondMessage);
                REQUIRE(std::get<2>(observer.tpl) == secondB);
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver with references in notify parameters")
    {
        TestMonoObservable<int&, std::array<wchar_t, 4>&> observable;
        TestRefObserver observer;

        int value = 123;
        int expectedValue = 456;
        std::array<wchar_t, 4> message = { L'T', L'e', L's', L't' };
        constexpr std::wstring_view expectedMessage(L"Done");

        WHEN("An observer is notified with references")
        {
            observable.attach(&observer);
            observable.notify(value, message);

            THEN("The observer receives references to the original values and changes it")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(value == expectedValue);
                REQUIRE(std::wstring_view(message.data(), message.size()) == expectedMessage);
            }
        }
    }
}