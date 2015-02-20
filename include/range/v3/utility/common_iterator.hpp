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
#ifndef RANGES_V3_UTILITY_COMMON_ITERATOR_HPP
#define RANGES_V3_UTILITY_COMMON_ITERATOR_HPP

#include <type_traits>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/meta.hpp>
#include <range/v3/utility/concepts.hpp>
#include <range/v3/utility/variant.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \cond
        namespace detail
        {
            template<typename I, typename S>
            struct common_cursor
            {
                using single_pass = SinglePass<I>;
                struct mixin
                  : basic_mixin<common_cursor>
                {
                    mixin() = default;
                    mixin(common_cursor pos)
                      : basic_mixin<common_cursor>{std::move(pos)}
                    {}
                    explicit mixin(I it)
                      : mixin(common_cursor{std::move(it)})
                    {}
                    explicit mixin(S se)
                      : mixin(common_cursor{std::move(se)})
                    {}
                };
            private:
                static_assert(!std::is_same<I, S>::value,
                              "Error: iterator and sentinel types are the same");
                tagged_variant<I, S> data_;
                bool is_sentinel() const
                {
                    RANGES_ASSERT(data_.is_valid());
                    return data_.which() == 1u;
                }
                I & it()
                {
                    RANGES_ASSERT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                I const & it() const
                {
                    RANGES_ASSERT(!is_sentinel());
                    return ranges::get<0>(data_);
                }
                S const & se() const
                {
                    RANGES_ASSERT(is_sentinel());
                    return ranges::get<1>(data_);
                }
                // The iterators of view::bounded are sized if I and S are
                // sized. This operator makes it so. It's templatized because
                // SizedIteratorRangeLike_ is expressed in terms of operator-.
                // The template breaks an cycle where templates are instantiated
                // before their definitions are available.
                template<typename I_ = I, typename S_ = S,
                    CONCEPT_REQUIRES_(SizedIteratorRangeLike_<I_, S_>())>
                friend iterator_difference_t<I>
                operator-(common_iterator<I, S> const &end, common_iterator<I, S> const &begin)
                {
                    common_cursor const &this_ = begin.mixin::get(), &that = end.mixin::get();
                    return that.is_sentinel() ?
                        (this_.is_sentinel() ? 0 : that.se() - this_.it()) :
                        (this_.is_sentinel() ?
                             that.it() - this_.se() :
                             that.it() - this_.it());
                }
                // BUGBUG TODO what about advance??
            public:
                common_cursor() = default;
                explicit common_cursor(I it)
                  : data_(meta::size_t<0>{}, std::move(it))
                {}
                explicit common_cursor(S se)
                  : data_(meta::size_t<1>{}, std::move(se))
                {}
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Convertible<I, I2>() &&
                                      Convertible<S, S2>())>
                operator common_cursor<I2, S2>() const
                {
                    return is_sentinel() ?
                        common_cursor<I2, S2>{S2{se()}} :
                        common_cursor<I2, S2>{I2{it()}};
                }
                auto current() const -> decltype(*std::declval<I const &>())
                {
                    return *it();
                }
                template<typename I2, typename S2,
                    CONCEPT_REQUIRES_(Common<I, I2>() && Common<S, S2>())>
                bool equal(common_cursor<I2, S2> const &that) const
                {
                    return is_sentinel() ?
                        (that.is_sentinel() || that.it() == se()) :
                        (that.is_sentinel() ?
                            it() == that.se() :
                            it() == that.it());
                }
                void next()
                {
                    ++it();
                }
            };
        }
        /// \endcond

        /// \addtogroup group-utility Utility
        /// @{
        ///
        template<typename Cur, typename S>
        struct common_type<basic_iterator<Cur, S>, basic_sentinel<S>>
        {
            using type =
                common_iterator<
                    basic_iterator<Cur, S>,
                    basic_sentinel<S>>;
        };

        template<typename Cur, typename S>
        struct common_type<basic_sentinel<S>, basic_iterator<Cur, S>>
        {
            using type =
                common_iterator<
                    basic_iterator<Cur, S>,
                    basic_sentinel<S>>;
        };

        template<typename Cur, typename S, typename TQual, typename UQual>
        struct common_reference_base<basic_iterator<Cur, S>, basic_sentinel<S>, TQual, UQual>
        {
            using type =
                common_iterator<
                    basic_iterator<Cur, S>,
                    basic_sentinel<S>>;
        };

        template<typename Cur, typename S, typename TQual, typename UQual>
        struct common_reference_base<basic_sentinel<S>, basic_iterator<Cur, S>, TQual, UQual>
        {
            using type =
                common_iterator<
                    basic_iterator<Cur, S>,
                    basic_sentinel<S>>;
        };
        /// @}
    }
}

#endif
