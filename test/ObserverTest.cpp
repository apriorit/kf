#include "pch.h"
#include <kf/patterns/Observer.h>
#include <string_view>
#include <optional>

namespace
{
    template<typename... T>
    class TestObserver : public kf::IObserver<T...>
    {
    public:
        void onNotify(T... t) override
        {
            m_values.emplace(t...);
        }
        std::optional<std::tuple<T...>> m_values;
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
            constexpr int kValue = 123;
            constexpr std::wstring_view kMessage(L"Test");

            observable.attach(&observer);
            observable.notify(kValue, kMessage);

            THEN("The observer is attached and receives the notification")
            {
                REQUIRE(observer.m_values.has_value());
                REQUIRE(std::get<0>(*observer.m_values) == kValue);
                REQUIRE(std::get<1>(*observer.m_values) == kMessage);
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, std::wstring_view> observable;
        TestObserver<int, std::wstring_view> observer;

        WHEN("An observer is detached")
        {
            constexpr int kValue = 123;
            constexpr std::wstring_view kMessage(L"Test");
            observable.attach(&observer);
            observable.detach(&observer);
            observable.notify(kValue, kMessage);

            THEN("The observer does not receive notifications")
            {
                REQUIRE(!observer.m_values.has_value());
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
            constexpr int kFirstA = 123;
            constexpr int kFirstB = 456;
            constexpr std::wstring_view kFirstMessage(L"First");

            observable.attach(&observer);
            observable.notify(kFirstA, kFirstMessage, kFirstB);
            THEN("First notification is received")
            {
                REQUIRE(observer.m_values.has_value());
                REQUIRE(std::get<0>(*observer.m_values) == kFirstA);
                REQUIRE(std::get<1>(*observer.m_values) == kFirstMessage);
                REQUIRE(std::get<2>(*observer.m_values) == kFirstB);
            }

            constexpr int kSecondA = 567;
            constexpr int kSecondB = 890;
            constexpr std::wstring_view secondMessage(L"Second");

            observable.notify(kSecondA, secondMessage, kSecondB);

            THEN("Second notification is received")
            {
                REQUIRE(observer.m_values.has_value());
                REQUIRE(std::get<0>(*observer.m_values) == kSecondA);
                REQUIRE(std::get<1>(*observer.m_values) == secondMessage);
                REQUIRE(std::get<2>(*observer.m_values) == kSecondB);
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver with references in notify parameters")
    {
        TestMonoObservable<int&, std::array<wchar_t, 4>&> observable;
        TestObserver<int&, std::array<wchar_t, 4>&> observer;

        int value = 123;
        constexpr int kExpectedValue = 456;
        std::array<wchar_t, 4> message = { L'T', L'e', L's', L't' };
        constexpr std::array<wchar_t, 4> kExpectedMessage = { L'D', L'o', L'n', L'e' };

        WHEN("An observer is notified with references")
        {
            observable.attach(&observer);
            observable.notify(value, message);

            std::get<0>(*observer.m_values) = kExpectedValue;
            std::get<1>(*observer.m_values) = kExpectedMessage;

            THEN("The observer receives references to the original values and changes it")
            {
                REQUIRE(observer.m_values.has_value());
                REQUIRE(value == kExpectedValue);
                REQUIRE(message == kExpectedMessage);
            }
        }
    }

    GIVEN("A TestMonoObservable without an attached observer")
    {
        TestMonoObservable<int> observable;

        WHEN("Notifying without observers")
        {
            THEN("No crashes occure")
            {
                observable.notify(123);
            }
        }
    }
}