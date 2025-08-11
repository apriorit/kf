#include "pch.h"
#include <kf/patterns/Observer.h>
#include <kf/UString.h>

class TestObserver : public kf::IObserver<int, kf::USimpleString>
{
public:
    void onNotify(int value, kf::USimpleString message) override
    {
        m_value = value;
        m_message.setString(message);
        m_isNotified = true;
    }

    int m_value = 0;
    kf::USimpleString m_message;
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

SCENARIO("Testing MonoObservable functionality")
{
    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, kf::USimpleString> observable;
        TestObserver observer;

        WHEN("An observer is attached")
        {
            observable.attach(&observer);

            THEN("The observer is attached without assertion")
            {
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, kf::USimpleString> observable;
        TestObserver observer;

        WHEN("An observer is notified")
        {
            observable.attach(&observer);
            observable.notify(123, L"Test");

            THEN("The observer receives the notification")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(observer.m_value == 123);
                REQUIRE(observer.m_message.equals(L"Test"));
            }
        }
    }

    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, kf::USimpleString> observable;
        TestObserver observer;

        WHEN("An observer is detached")
        {
            observable.attach(&observer);
            observable.detach(&observer);
            observable.notify(123, L"Test");

            THEN("The observer does not receive notifications")
            {
                REQUIRE(observer.m_isNotified == false);
            }
        }
    }

    GIVEN("A TestMonoObservable")
    {
        TestMonoObservable<int, kf::USimpleString> observable;
        TestObserver observer;

        WHEN("No observer is attached and notify is called")
        {
            observable.notify(123, L"Test");

            THEN("Nothing happens and no notification is sent")
            {
                REQUIRE(!observer.m_isNotified);
            }
        }
    }


    GIVEN("A TestMonoObservable and a TestObserver")
    {
        TestMonoObservable<int, kf::USimpleString> observable;
        TestObserver observer;

        WHEN("Multiple notifications are sent")
        {
            observable.attach(&observer);
            observable.notify(1, L"First");
            THEN("First notification is received")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(observer.m_value == 1);
                REQUIRE(observer.m_message.equals(L"First"));
            }

            observable.notify(2, L"Second");

            THEN("Second notification is received")
            {
                REQUIRE(observer.m_isNotified);
                REQUIRE(observer.m_value == 2);
                REQUIRE(observer.m_message.equals(L"Second"));
            }
        }
    }
}