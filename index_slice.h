#pragma once
//
// Created by Miles Gibson on 22/02/17.
//

// ============
#include "slice.hpp"

namespace mkz {

    template <typename T>
    struct index_slice {

        size_t start;
        size_t count;

        inline size_t size() const noexcept { return count; }


    };

    template <typename T, typename Container>
    mkz::slice<T> to_slice(const index_slice<T>& s, Container& c) {
        return mkz::make_slice(&c[s.start], s.count);
    }

    template <typename T, typename Container>
    mkz::slice<const T> to_slice(const index_slice<T>& s, const Container& c) {
        return mkz::make_slice(&c[s.start], s.count);
    }

}

