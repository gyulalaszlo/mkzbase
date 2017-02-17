#pragma once


#include <memory>

#include "mkz.h"

namespace mkz {

    template<typename A>
    class Maybe;

    template<typename A>
    inline Maybe<A> just(A a);

    template<typename A>
    inline Maybe<A> nothing();

// shortcut for removing nothing<decltype(something)>() to nothing(something)
    template<typename A>
    inline Maybe<A> nothing(const A &sampleT);

    template<typename A>
    class Maybe {
    public:

        Maybe() : kind(kNOTHING) {}

        Maybe(A a) : _just(std::move(a)), kind(kJUST) {}

        // disallow copying for now (only moves)
        Maybe(const Maybe &other) = delete;

        Maybe &operator=(const Maybe &other) = delete;

        Maybe(Maybe &&other) = default;

        Maybe &operator=(Maybe &&other) = default;

        inline bool ok() const { return kind == kJUST; }

        template<typename D>
        inline const Maybe<A> &onJust(D delegate) const {
            if (ok()) { delegate(_just); }
            return *this;
        }

        template<typename D>
        inline const Maybe<A> &onNothing(D delegate) const {
            if (!ok()) { delegate(); }
            return *this;
        }


        // Chain maybes
        template<typename D>
        inline auto then(D delegate) const {
            return ok()
                   ? delegate(_just)
                   : nothing<decltype(delegate(_just))>();
        }


    private:
        enum Kind {
            kNOTHING = 0, kJUST = 1,
        };

        union {
            A _just;
        };
        Kind kind;
    };

    template<typename A>
    inline Maybe<A> just(A a) { return Maybe<A>(std::move(a)); }

    template<typename A>
    inline Maybe<A> nothing() { return Maybe<A>(); }

// shortcut for removing nothing<decltype(something)>() to nothing(something)
    template<typename A>
    inline Maybe<A> nothing(const A &sampleT) { return Maybe<A>(); }

}

