#pragma once
//
// Created by Miles Gibson on 23/02/17.
//

#include <iterator>

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

}

