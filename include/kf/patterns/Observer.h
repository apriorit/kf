#pragma once
#include <kf/stl/cassert>

namespace kf
{
    template<typename... T>
    class IObserver
    {
    public:
        virtual ~IObserver() = default;

        virtual void onNotify(T... t) = 0;
    };

    template<typename... T>
    class IObservable
    {
    public:
        virtual ~IObservable() = default;

        virtual void attach(IObserver<T...>*) = 0;
        virtual void detach(IObserver<T...>*) = 0;
    };

    template<typename... T>
    class MonoObservable : public IObservable<T...>
    {
    public:
        void attach(IObserver<T...>* observer) override
        {
            assert(!m_observer);
            m_observer = observer;
        }

        void detach(IObserver<T...>* observer) override
        {
            assert(!m_observer || m_observer == observer);
            static_cast<void>(observer);
            m_observer = nullptr;
        }

    protected:
        void notify(T... t)
        {
            if (m_observer)
            {
                m_observer->onNotify(t...);
            }
        }

    private:
        IObserver<T...>* m_observer = nullptr;
    };
}
