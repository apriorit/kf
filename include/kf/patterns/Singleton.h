#pragma once

namespace kf
{
    template<class T>
    class Singleton
    {
    public:
        Singleton()
        {
            assert(!m_instance);
            m_instance = static_cast<T*>(this);
        }

        ~Singleton()
        {
            assert(m_instance);
            m_instance = nullptr;
        }

        static T* getInstance()
        {
            return m_instance;
        }

    private:
        static T* m_instance;
    };

    template<class T>
    T* Singleton<T>::m_instance = nullptr;
}
