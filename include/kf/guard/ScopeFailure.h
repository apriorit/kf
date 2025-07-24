#pragma once
#include <functional>

#define SCOPE_FAILURE_CAT2(x, y) x##y
#define SCOPE_FAILURE_CAT(x, y) SCOPE_FAILURE_CAT2(x, y)
#define SCOPE_FAILURE(status) auto SCOPE_FAILURE_CAT(scopeFailure_, __COUNTER__) = kf::MakeScopeFailure(status) += [&]

namespace kf
{
    template<typename F>
    class ScopeFailure
    {
    public:
        ScopeFailure(NTSTATUS& status, F&& f) : m_status(status), m_f(f)
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
        NTSTATUS& m_status;
        F m_f;        
    };

    struct MakeScopeFailure
    {
        MakeScopeFailure(NTSTATUS& status) : m_status(status)
        {
        }

        template<typename F>
        ScopeFailure<F> operator+=(F&& f)
        {
            return ScopeFailure<F>(m_status, std::move(f));
        }

        NTSTATUS& m_status;
    };
}
