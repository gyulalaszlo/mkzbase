cmake_minimum_required(VERSION 3.6)
project(mkzbase CXX C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


add_library( mkzbase
        murmur3/murmur3.h
        murmur3/murmur3.c

        optional/optional.hpp

        variant/include/mapbox/variant.hpp
        variant/include/mapbox/variant_io.hpp
        variant/include/mapbox/variant_visitor.hpp
        variant/include/mapbox/recursive_wrapper.hpp

        compose.h
        maybe.h
        mkz.h
        optional.h
        scopeguard.h
        slice.hpp
        slice_copy.hpp
        variant.h
        index_slice.h
        mkz-algorithm.h

        )

target_include_directories(mkzbase
        PUBLIC . ./optional ./variant/include)


set(MKZBASE_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/variant/include

        PARENT_SCOPE)

#include_directories("variant/include")
#include_directories("optional")

