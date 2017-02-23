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
        inline size_t end() const noexcept { return start + count; }

        template <typename U>
        inline index_slice<U> as() const { return {start, count}; }
    };


    template <typename Container, typename T>
    struct index_slice_iterator {

        // Iterator traits, previously from std::iterator.
        using value_type = T;
        using difference_type = std::size_t;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

        // Default constructible.
        index_slice_iterator() = default;
        explicit index_slice_iterator(Container& c, size_t start) : c(c), index(start) {}

        // Dereferencable.
        const T& operator*() const {
            return c[index];
        }


        // Dereferencable.
        reference operator*()  {
            return c[index];
        }

        // Pre- and post-incrementable.
        index_slice_iterator& operator++() { index++; return *this; }
        index_slice_iterator operator++(int n)  { index += n; return *this; }

//        // Pre- and post-decrementable.
        index_slice_iterator& operator--() { index--; return *this; };
        index_slice_iterator operator--(int n) { index -=  n; return *this;};

        // Equality / inequality.
        bool operator==(const index_slice_iterator& rhs) {
            return index == rhs.index;
        }

        bool operator!=(const index_slice_iterator& rhs) {
            return index != rhs.index;
        }
        Container& c;
        size_t index;
    };


    template <typename T, typename Container>
    struct index_slice_with_container {
        using value_type = T;
        using iterator = index_slice_iterator<Container, T>;
        using const_iterator = index_slice_iterator<const Container, const T>;
        using slice_t = index_slice<T>;


        Container& c;
        index_slice<T> s;


        iterator begin() { return iterator(c, s.start); };
        iterator end() { return iterator(c, s.end()); };

        const_iterator begin() const { return const_iterator(c, s.start); };
        const_iterator end() const { return const_iterator(c, s.end()); };


        T& operator[](size_t idx) { return c[s.start + idx]; }
        const T& operator[](size_t idx) const { return c[s.start + idx]; }

        index_slice<T> slice() const { return s; }

        size_t size() const { return s.size(); }
    };


    template <typename T, typename Container>
    mkz::slice<T> to_slice(const index_slice<T>& s, Container& c) {
        return mkz::make_slice(&c[s.start], s.count);
    }

    template <typename T, typename Container>
    mkz::slice<const T> to_slice(const index_slice<T>& s, const Container& c) {
        return mkz::make_slice(&c[s.start], s.count);
    }

    template <typename T,  typename Container>
    mkz::index_slice_with_container<T, Container> with_container(const index_slice<T> s, Container& c) {
        return mkz::index_slice_with_container< T, Container> { c, s };
    }

    template <typename T, typename Container>
    mkz::index_slice_with_container<const T, const Container> with_container(index_slice<T> s, const Container& c) {
//        return mkz::index_slice_with_container< T, const Container> { c, s };
        return mkz::index_slice_with_container<const T, const Container> { c, s.template as<const T>() };
    }

    template <typename T, typename Container>
    mkz::index_slice_with_container<T, Container> indexed_slice_from_append(Container& c, size_t n) {
        size_t start = c.size();
        c.resize(start + n);
        return mkz::index_slice_with_container< T, Container> { c, { start, n } };
    }


    // TRANSFORM

    // std::transform dereferences the output iterator more then needed, hence this fn
    // (index_slice has been reallocated from under it)
    template <typename _InputIterator, typename _OutputIterator, typename _UnaryOperation>
    _OutputIterator transform_(_InputIterator __first, _InputIterator __last, _OutputIterator __result, _UnaryOperation __op)
    {
        for (; __first != __last; ++__first, (void) ++__result) {
            auto&& res = __op(*__first);
            *__result = res;
        }
        return __result;
    }


    template <typename T, typename InputIt, typename Container, typename Fn>
    mkz::index_slice_with_container<T, Container> indexed_slice_from_append(InputIt&& beg, InputIt&& end, Container& c, Fn&& fn) {
        auto s = indexed_slice_from_append<T>( c, end - beg );
        transform_( beg, end, s.begin(), fn);
        return s;
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

