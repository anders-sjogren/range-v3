/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//

#ifndef RANGES_V3_VIEW_REPEAT_N_HPP
#define RANGES_V3_VIEW_REPEAT_N_HPP

#include <range/v3/range_fwd.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_facade.hpp>
#include <range/v3/utility/static_const.hpp>

namespace ranges
{
    inline namespace v3
    {
        /// \addtogroup group-views
        /// @{

        // Ordinarily, a view shouldn't contain its elements. This is so that copying
        // and assigning ranges is O(1), and also so that in the event of element
        // mutation, all the copies of the range see the mutation the same way. The
        // repeat_view *does* own its lone element, though. This is OK because:
        //  - O(N) copying is fine when N==1 as it is in this case, and
        //  - The element is immutable, so there is no potential for incorrect
        //    semantics.
        template<typename Val>
        struct repeat_n_view
          : range_facade<repeat_n_view<Val>>
        {
        private:
            friend range_access;
            Val value_;
            std::size_t n_;

            struct cursor
            {
            private:
                Val value_;
                std::size_t n_;
            public:
                using single_pass = std::true_type;
                cursor() = default;
                cursor(Val value, std::size_t n)
                  : value_(std::move(value)), n_(n)
                {}
                Val current() const
                {
                    return value_;
                }
                constexpr bool done() const
                {
                    return 0 == n_;
                }
                void next()
                {
                    RANGES_ASSERT(0 != n_);
                    --n_;
                }
            };
            cursor begin_cursor() const
            {
                return {value_, n_};
            }
        public:
            repeat_n_view() = default;
            constexpr repeat_n_view(Val value, std::size_t n)
              : value_(detail::move(value)), n_(n)
            {}
            constexpr std::size_t size() const
            {
                return n_;
            }
        };

        namespace view
        {
            struct repeat_n_fn
            {
                template<typename Val,
                    CONCEPT_REQUIRES_(SemiRegular<Val>())>
                repeat_n_view<Val> operator()(Val value, std::size_t n) const
                {
                    return repeat_n_view<Val>{std::move(value), n};
                }
            #ifndef RANGES_DOXYGEN_INVOKED
                template<typename Val,
                    CONCEPT_REQUIRES_(!SemiRegular<Val>())>
                void operator()(Val, std::size_t) const
                {
                    CONCEPT_ASSERT_MSG(SemiRegular<Val>(),
                        "The value passed to view::repeat_n must be SemiRegular; that is, it needs "
                        "to be default constructable, copy and move constructable, and destructable.");
                }
            #endif
            };

            /// \relates repeat_n_fn
            /// \ingroup group-views
            namespace
            {
                constexpr auto&& repeat_n = static_const<repeat_n_fn>::value;
            }
        }
        /// @}
    }
}

#endif
