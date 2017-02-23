#pragma once
//
// Created by Miles Gibson on 23/02/17.
//

#include <iterator>
#include "index_slice.h"

namespace mkz {

    template<typename Iter, typename Func>
    void combine_pairwise(Iter&& first, Iter&& last, Func&& func)
    {
        for(; first != last; ++first) {
            for(Iter next = std::next(first); next != last; ++next)
            {
                if (!func(first, next)) return;
            }
        }
    }

    // TREE TO LINEAR ARRAY
    // ====================



    template <typename T, typename InputContainer, typename Container, typename State, typename MapFn>
    mkz::index_slice<T> tree_to_linear_map(
            const InputContainer& i,
            Container& o,
            const State& state,
            MapFn&& mapFn
    ) {

        auto recurse = [&](auto&& state, auto&& children) {
            return tree_to_linear_map<T>(children, o, state, mapFn);
        };

        auto s = mkz::indexed_slice_from_append<T>( o, i.size() );
        transform_( i.begin(), i.end(), s.begin(), [&](auto&& e){
            return mapFn(state, e, recurse);
        });

        return s.slice();
    }


}

