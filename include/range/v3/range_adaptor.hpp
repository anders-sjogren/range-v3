// Copyright Eric Niebler 2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//
#ifndef RANGES_V3_RANGE_ADAPTOR_HPP
#define RANGES_V3_RANGE_ADAPTOR_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/range_traits.hpp>

namespace ranges
{
    inline namespace v3
    {
        template<typename Derived, typename BaseIterable, bool Infinite>
        struct range_adaptor
          : range_facade<Derived, Infinite>
        {
        protected:
            using range_adaptor_ = range_adaptor;
            using range_facade<Derived, Infinite>::derived;

            range_adaptor &adaptor()
            {
                return *this;
            }
            range_adaptor const &adaptor() const
            {
                return *this;
            }
        private:
            BaseIterable rng_;
            friend range_core_access;
            friend Derived;

            template<bool Const>
            struct basic_sentinel_impl;

            // Give BaseIterable::iterator a simple interface for passing to Derived
            template<bool Const>
            struct basic_impl
            {
            private:
                friend struct range_adaptor;
                template<bool OtherConst>
                friend struct basic_sentinel_impl;
                using base_range = detail::add_const_if_t<BaseIterable, Const>;
                using base_range_iterator = range_iterator_t<base_range>;
                base_range_iterator it_;
                constexpr basic_impl(base_range_iterator it)
                  : it_(detail::move(it))
                {}
            public:
                constexpr basic_impl()
                  : it_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst || Const> = 0>
                constexpr basic_impl(basic_impl<OtherConst> that)
                  : it_(detail::move(that.it_))
                {}
                basic_impl &base()
                {
                    return *this;
                }
                basic_impl const &base() const
                {
                    return *this;
                }
                void next()
                {
                    ++it_;
                }
                auto current() const -> decltype(*it_)
                {
                    return *it_;
                }
                template<bool OtherConst>
                bool equal(basic_impl<OtherConst> const &that) const
                {
                    return it_ == that.it_;
                }
                template<bool OtherConst>
                bool equal(basic_sentinel_impl<OtherConst> const &that) const
                {
                    return that.equal(*this);
                }
                CONCEPT_REQUIRES(BidirectionalIterable<BaseIterable>())
                void prev()
                {
                    --it_;
                }
                CONCEPT_REQUIRES(RandomAccessIterable<BaseIterable>())
                void advance(range_difference_t<BaseIterable> n)
                {
                    it_ += n;
                }
                template<bool OtherConst,
                         CONCEPT_REQUIRES_(RandomAccessIterable<BaseIterable>())>
                range_difference_t<BaseIterable> distance_to(basic_impl<OtherConst> const &that) const
                {
                    return that.it_ - it_;
                }
            };

            template<bool Const>
            struct basic_sentinel_impl
            {
            private:
                template<bool OtherConst>
                friend struct basic_impl;
                friend struct range_adaptor;
                using base_range = detail::add_const_if_t<BaseIterable, Const>;
                using base_range_sentinel = range_sentinel_t<base_range>;
                base_range_sentinel end_;
                constexpr basic_sentinel_impl(base_range_sentinel end)
                  : end_(detail::move(end))
                {}
            public:
                constexpr basic_sentinel_impl()
                  : end_{}
                {}
                // For iterator -> const_iterator conversion
                template<bool OtherConst, enable_if_t<!OtherConst || Const> = 0>
                constexpr basic_sentinel_impl(basic_sentinel_impl<OtherConst> that)
                  : end_(detail::move(that.end_))
                {}
                basic_sentinel_impl &base()
                {
                    return *this;
                }
                basic_sentinel_impl const &base() const
                {
                    return *this;
                }
                template<bool OtherConst>
                constexpr bool equal(basic_impl<OtherConst> const &that) const
                {
                    return that.it_ == end_;
                }
            };

            template<bool Const>
            using basic_sentinel =
                detail::conditional_t<
                    (Range<BaseIterable>())
                  , basic_impl<Const>
                  , basic_sentinel_impl<Const>>;

            basic_impl<false> begin_impl()
            {
                return {ranges::begin(rng_)};
            }
            basic_impl<true> begin_impl() const
            {
                return {ranges::begin(rng_)};
            }
            basic_sentinel<false> end_impl()
            {
                return {ranges::end(rng_)};
            }
            basic_sentinel<true> end_impl() const
            {
                return {ranges::end(rng_)};
            }

        public:
            constexpr range_adaptor(BaseIterable && rng)
              : rng_(detail::forward<BaseIterable>(rng))
            {}
            BaseIterable & base()
            {
                return rng_;
            }
            BaseIterable const & base() const
            {
                return rng_;
            }
        };
    }
}

#endif
