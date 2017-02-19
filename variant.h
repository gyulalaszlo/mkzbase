#pragma once

#include "variant/include/mapbox/variant.hpp"

namespace mkz {
    template <typename ...Args>
    using variant = mapbox::util::variant<Args...>;
}