#pragma once

#include <array>

namespace mkz {

    // Composes two functors togeather
    template <typename A, typename B>
    struct compose_t {
        A a;
        B b;

        template <typename C>
        constexpr decltype(auto) operator()(C&& c) const  {
            return std::forward<decltype(a(b(c)))>(a(b(c)));
        }
    };

    template <typename A, typename B>
    inline constexpr compose_t<A,B> compose(A a, B b) {
        return  compose_t<A,B> {a,b};
    }


    template <int i>
    struct identity_t {
        // identity function with perfect forwarding
        template<typename..., typename V>
        constexpr V&& operator()(V&& v) const {
            return std::forward<V>(v);
        }
    };

    struct identity_mapper_t : public identity_t<1> { };
    struct identity_reducer_t : public identity_t<2> { };

    template <typename Op, typename Mapper = identity_mapper_t, typename Reducer = identity_reducer_t>
    struct pipe_t : private Op, private Mapper, private Reducer {

        pipe_t(Op op, Mapper m = Mapper(), Reducer r = Reducer()) : Op(std::move(op)), Mapper(std::move(m)), Reducer(std::move(r)) {}

        template <typename C>
        constexpr decltype(auto) operator()(C&& c) const  {
            return std::forward<decltype(Mapper::operator()(c, Op::operator(), Reducer::operator()))>(
                    Mapper::operator()(c, Op::operator(), Reducer::operator())
            );
        }
    };



    decltype(auto)  make_pipe(){
        return pipe_t<identity_t<0>>(identity_t<0>{});
    }

    // builds a pipe
    template <typename Fst, typename...Args>
    decltype(auto) make_pipe(Fst f, Args&&... args){
        using Op = decltype(compose(make_pipe(args...), f ));
        return pipe_t<Op>(compose(make_pipe(args...), f ));
    }


    // applying the pipe
    // =================

    namespace pipes {


        namespace impl {
            // Wraps an iterator in a pipeline
            template <typename Pipe, typename BaseIter>
            struct piped_iterator : public BaseIter {
                Pipe& pipe;

                piped_iterator(Pipe& pipe, BaseIter&& bit) : BaseIter(std::move(bit)), pipe(pipe) {}
            };


            // implementation details, users never invoke these directly
            namespace detail
            {
                template <typename F, typename Tuple, bool Done, int Total, int... N>
                struct call_impl
                {
                    static void call(F f, Tuple && t)
                    {
                        call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
                    }
                };

                template <typename F, typename Tuple, int Total, int... N>
                struct call_impl<F, Tuple, true, Total, N...>
                {
                    static void call(F f, Tuple && t)
                    {
                        f(std::get<N>(std::forward<Tuple>(t))...);
                    }
                };
            }

            template <typename F, typename Tuple>
            void call(F f, Tuple && t)
            {
                typedef typename std::decay<Tuple>::type ttype;
                detail::call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(f, std::forward<Tuple>(t));
            }

            // zipping tuples

            // index_apply

            template <class F, size_t... Is>
            constexpr auto index_apply_impl(F f, std::index_sequence<Is...>) {
                return f(std::integral_constant<size_t, Is> {}...);
            }

            template <size_t N, class F>
            constexpr auto index_apply(F f) {
                return index_apply_impl(f, std::make_index_sequence<N>{});
            }


            //

            template<class Tuple>
            constexpr auto reverse(Tuple t) {
                using namespace std;
                return index_apply<tuple_size<Tuple> {}>(
                        [&](auto... Is) {
                            return make_tuple(
                                    get<std::tuple_size<Tuple>{} - 1 - Is>(t)...);
                        });
            }

            template <class... Tuples>
            constexpr auto zip(Tuples... ts) {
                using namespace std;
                constexpr size_t len = min({tuple_size<Tuples>{}...});
                auto row =
                        [&](auto I) { return make_tuple(get<I>(ts)...); };
                return index_apply<len>(
                        [&](auto... Is) { return make_tuple(row(Is)...); });
            }

            constexpr auto zip() { return std::make_tuple(); }


            /////

            // combines a number of pipes with a function
            template <typename CombinerFn, typename...Pipes>
            struct pipe_combiner {
                CombinerFn combinerFn;
                std::tuple<Pipes...> pipes;


                template <typename... SrcArgs>
                decltype(auto) operator()(SrcArgs&&... args) {
                    call(combinerFn, index_apply<sizeof...(Pipes)>([&](auto... Is){
                        return std::make_tuple(std::get<Is>( pipes )( std::get<Is>( args ) )...);
                    }));
                }
            };


            //===================


            // combines a number of pipes with a function
            template <typename Collection, typename Pipe, typename Mapper, typename Reducer >
            struct pipe_over_collection : private Mapper, private Reducer {
                Pipe pipe;

                decltype(auto) operator()(Collection&& c) {
                    return reducer(mapper(c, pipe));
                }
            };

        }

        /// Adds the pipe on top of an iterator
        template <typename Pipe, typename BaseIter>
        impl::piped_iterator<Pipe, BaseIter> on_iterator( Pipe&& pipe, BaseIter&& it ) {
            return { std::forward<Pipe>(pipe), std::forward<BaseIter>(it) };
        }


        /// Combines a number of pipeplines into one
        template <typename CombinerPipe, typename... Pipes>
        decltype(auto) combine(CombinerPipe&& fn, Pipes&&... args) {
            return impl::pipe_combiner<CombinerPipe, Pipes...>{fn,  std::tuple<Pipes...>{ args...}};
        }


        // PAIR ADAPTOR
        // ============

        struct std_array_mapper_t {

            template <typename Arr, typename Op, typename ReducerFactory>
            decltype(auto) operator()(Arr&& p, Op&& op, ReducerFactory&& reducerFactory) const {

                using T = typename Arr::value_type;
                using Out = decltype(op(std::forward<T>(p[0])));

                auto reducer = reducerFactory(std::tuple_size<Arr>{}.value);

                for (auto&& e : p) {
                    reducer( std::forward<Out>( op(std::move(e)) ));
                }

                return reducer();
//                return std::make_pair(std::forward<A>(p.first), std::forward<B>(p.second));
            }
        };

        template <size_t N, typename T>
        struct std_array_reducer_t {
            using value_type = std::array<T,N>;


            value_type els;
            size_t idx;

            // step
            constexpr void operator()(T&& p) {
                assert(idx < N);
                els[idx] = std::move(p);
                ++idx;
            }

            // finalize
            constexpr value_type operator()() {
                assert(idx == N - 1);
                // move the data out of the array
                return std::move(els);
            }

        };




        template <size_t tuple_size>
        struct std_tuple_reducer {

            template <typename T>
            std_tuple_reducer_t<tuple_size, T> operator()(size_t size_guess) const {
                return std_tuple_reducer_t<tuple_size, T>();
            }
        };

//
//
//        template <size_t N>
//        using std_pair_reducer = reducer_factory<std_tuple_reducer_t>;


    }
}
