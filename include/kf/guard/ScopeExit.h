#pragma once
#include <functional>

#define SCOPE_EXIT_CAT2(x, y) x##y
#define SCOPE_EXIT_CAT(x, y) SCOPE_EXIT_CAT2(x, y)
#define SCOPE_EXIT auto SCOPE_EXIT_CAT(scopeExit_, __COUNTER__) = kf::MakeScopeExit() += [&]

namespace kf
{
    template<typename F>
    class ScopeExit 
    {
    public:
        ScopeExit(F&& f) : m_f(f)
        {
        }
        
        ~ScopeExit() 
        { 
            m_f();
        }

        ScopeExit(ScopeExit&& other);

    private:
        ScopeExit(const ScopeExit&);
        ScopeExit& operator=(const ScopeExit&);

    private:
        F m_f;
    };

    struct MakeScopeExit
    {
        template<typename F>
        ScopeExit<F> operator+=(F&& f)
        {
            return ScopeExit<F>(std::move(f));
        }
    };
}
