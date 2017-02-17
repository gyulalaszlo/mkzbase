#pragma once

namespace mkz {

    template <typename A, typename B>
    struct compose_t {
        A a;
        B b;
        template <typename C>
        auto operator()(C c) const -> decltype(a(b(c))) {
            return a(b(c));
        }
    };

    template <typename A, typename B>
    compose_t compose(A a, B b) {
        return  compose_t<A,B> {a,b};
    }

}
