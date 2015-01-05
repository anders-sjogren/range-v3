/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_FOR_EACH_HPP
#define RANGES_V3_ALGORITHM_FOR_EACH_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/begin_end.hpp>
#include <range/v3/range_traits.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/utility/invokable.hpp>
#include <range/v3/utility/functional.hpp>
#include <range/v3/utility/iterator_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace adl_detail
        {
            template<typename I, typename S, typename F, typename P = ident,
                typename V = iterator_common_reference_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(InputIterator<I>() && IteratorRange<I, S>() &&
                    Invokable<P, V>() && Invokable<F, X>())>
            I for_each(I begin, S end, F fun_, P proj_ = P{})
            {
                auto &&fun = invokable(fun_);
                auto &&proj = invokable(proj_);
                for(; begin != end; ++begin)
                {
                    fun(proj(*begin));
                }
                return begin;
            }

            template<typename Rng, typename F, typename P = ident,
                typename I = range_iterator_t<Rng>,
                typename V = iterator_common_reference_t<I>,
                typename X = concepts::Invokable::result_t<P, V>,
                CONCEPT_REQUIRES_(InputIterable<Rng &>() && Invokable<P, V>() &&
                    Invokable<F, X>())>
            I for_each(Rng &&rng, F fun, P proj = P{})
            {
                return for_each(begin(rng), end(rng), std::move(fun), std::move(proj));
            }

            struct for_each_fn
            {
                template <typename... Args>
                constexpr auto operator()(Args&&... args) const ->
                decltype(for_each(std::forward<Args>(args)...))
                {
                    return for_each(std::forward<Args>(args)...);
                }
            };
        }
        /// \endcond

        /// \ingroup group-algorithms
        constexpr adl_detail::for_each_fn for_each{};
    } // namespace v3
} // namespace ranges

#endif // include guard
