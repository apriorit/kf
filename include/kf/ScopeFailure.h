#pragma once
#include <functional>

#define SCOPE_FAILURE_CAT2(x, y) x##y
#define SCOPE_FAILURE_CAT(x, y) SCOPE_FAILURE_CAT2(x, y)
#define SCOPE_FAILURE(status) auto SCOPE_FAILURE_CAT(scopeFailure_, __COUNTER__) = kf::MakeScopeFailure(status) += [&]

namespace kf
{
    template<typename F, typename T> requires std::convertible_to<T, NTSTATUS>
    class ScopeFailure
    {
    public:
        ScopeFailure(T& status, F&& f) : m_status(status), m_f(f)
        {
        }
        
        ~ScopeFailure()
        { 
            if (!NT_SUCCESS(m_status))
            {
                m_f();
            }
        }

        ScopeFailure(ScopeFailure&& other);

    private:
        ScopeFailure(const ScopeFailure&);
        ScopeFailure& operator=(const ScopeFailure&);

    private:
        T& m_status;
        F m_f;
    };

    template<typename T> requires std::convertible_to<T, NTSTATUS>
    struct MakeScopeFailure
    {
        MakeScopeFailure(T& status) : m_status(status)
        {
        }

        template<typename F>
        ScopeFailure<F, T> operator+=(F&& f)
        {
            return ScopeFailure<F, T>(m_status, std::move(f));
        }

        T& m_status;
    };
}
