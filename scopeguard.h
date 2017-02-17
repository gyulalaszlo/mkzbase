#pragma once


#include <utility>

namespace mkz {

    // on scope exit -- instead of destructors

    template<typename D>
    class ScopeGuard {
        friend ScopeGuard<D> onScopeExit(D d);

    public:
        ~ScopeGuard() { d(); }

        ScopeGuard(D d) : d(std::move(d)) {}


        ScopeGuard(const ScopeGuard &other) = delete;

        ScopeGuard(ScopeGuard &&other) = default;

        ScopeGuard &operator=(const ScopeGuard &other) = delete;

        ScopeGuard &operator=(ScopeGuard &&other) = default;

    private:
        D d;
    };

    template<typename D>
    ScopeGuard<D> onScopeExit(D d) { return ScopeGuard<D>(d); }
}

#define defer(x) const auto _scopeGuard ## __LINE__  = mkz::onScopeExit(x)
