#pragma once

#ifndef OCT_UTILITY
#define OCT_UTILITY

#include <initializer_list>
#include <utility>
#include "type_traits.hpp"

namespace oct {
    template <typename Ty>
    void swap(Ty& left, Ty& right) {
        Ty temp(left);
        left = right;
        right = std::move(temp);
    }

    template <typename Ty1, typename Ty2>
    struct pair {
        using first_type  = Ty1;
        using second_type = Ty2;

        first_type first;
        second_type second;

        template <typename Ref1, typename Ref2>
        pair(Ref1&& target1, Ref2&& target2) 
            : first(std::forward<Ref1>(target1)), second(std::forward<Ref2>(target2)) {}
        
        pair(const std::pair<Ty1, Ty2>& std_pair) 
            : first(std_pair.first), second(std_pair.second) {}

        pair()                       = default;
        pair(const pair&)            = default;
        pair(pair&&)                 = default;
        pair& operator=(const pair&) = default;
        pair& operator=(pair&&)      = default;
    };

    template <typename Ty1, typename Ty2>
    bool operator==(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return left.first == right.first && left.second == right.second;
    }

    template <typename Ty1, typename Ty2>
    bool operator!=(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return !(left.first == right.first) || !(left.second == right.second);
    }

    template <typename Ty1, typename Ty2>
    bool operator<(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return !(left.first == right.first) ? left.first < right.first : left.second < right.second;
    }

    template <typename Ty1, typename Ty2>
    bool operator<=(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return left < right || left == right;
    }

    template <typename Ty1, typename Ty2>
    bool operator>(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return !(left <= right);
    }

    template <typename Ty1, typename Ty2>
    bool operator>=(const pair<Ty1, Ty2>& left, const pair<Ty1, Ty2>& right) noexcept {
        return !(left < right);
    }

    template <typename Ty>
    struct key_of {
        using type = Ty;
    };

    template <typename Ty1, typename Ty2>
    struct key_of<pair<Ty1, Ty2>> {
        using type = Ty1;
    };

    template <typename Ty>
    using key_of_v = typename key_of<Ty>::type;

    template <typename Ty>
    struct value_of {
        using type = Ty;
    };

    template <typename Ty1, typename Ty2>
    struct value_of<pair<Ty1, Ty2>> {
        using type = Ty2;
    };

    template <typename Ty>
    using value_of_v = typename value_of<Ty>::type;

    template <typename Ty1, typename Ty2, bool = std::is_empty_v<Ty1> && !std::is_final_v<Ty1>>
    struct compressed_pair final : public Ty1 {
        using first_type  = Ty1;
        using second_type = Ty2;

        second_type second;

        compressed_pair() = default;

        template <typename First, class... Args2>
        constexpr explicit compressed_pair(First&& args1, Args2&&... args2)
            : Ty1(std::forward<First>(args1)), second(std::forward<Args2>(args2)...) {}

        constexpr first_type& get_first() noexcept {
            return *this;
        }
    };

    template <typename Ty1, typename Ty2>
    struct compressed_pair<Ty1, Ty2, false> final {
        using first_type = Ty1;
        using second_type = Ty2;

        first_type first;
        second_type second;

        compressed_pair() = default;

        template <typename First, class... Args2>
        constexpr explicit compressed_pair(First&& args1, Args2&&... args2)
            : first(std::forward<First>(args1)), second(std::forward<Args2>(args2)...) {}

        constexpr first_type& get_first() noexcept {
            return first;
        }
    };
}

#ifndef OCT_PAIR_STRUCTURED_BINDING
#define OCT_PAIR_STRUCTURED_BINDING

namespace std {
    template<typename Ty1, typename Ty2>
    struct tuple_size<oct::pair<Ty1, Ty2>> :
        integral_constant<size_t, 2> {};

    template<size_t Index, typename Ty1, typename Ty2>
    struct tuple_element<Index, oct::pair<Ty1, Ty2>> {
        using type = Ty1;
    };

    template<typename Ty1, typename Ty2>
    struct tuple_element<1, oct::pair<Ty1, Ty2>> {
        using type = Ty2;
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<!Idx, int> = 0>
    constexpr Ty1& get(oct::pair<Ty1, Ty2>& p) noexcept {
        return p.first;
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<Idx, int> = 0>
    constexpr Ty2& get(oct::pair<Ty1, Ty2>& p) noexcept {
        return p.second;
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<!Idx, int> = 0>
    constexpr const Ty1& get(const oct::pair<Ty1, Ty2>& p) noexcept {
        return p.first;
    };

    template <size_t Idx,typename Ty1, typename Ty2, enable_if_t<Idx, int> = 0>
    constexpr const Ty2& get(const oct::pair<Ty1, Ty2>& p) noexcept {
        return p.second;
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<!Idx, int> = 0>
    constexpr Ty1&& get(oct::pair<Ty1, Ty2>&& p) noexcept {
        return move(p.first);
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<Idx, int> = 0>
    constexpr Ty2&& get(oct::pair<Ty1, Ty2>&& p) noexcept {
        return move(p.second);
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<!Idx, int> = 0>
    constexpr const Ty1&& get(const oct::pair<Ty1, Ty2>&& p) noexcept {
        return move(p.first);
    };

    template <size_t Idx, typename Ty1, typename Ty2, enable_if_t<Idx, int> = 0>
    constexpr const Ty2&& get(const oct::pair<Ty1, Ty2>&& p) noexcept {
        return move(p.second);
    };
}

#endif // !OCT_PAIR_STRUCTURED_BINDING

#endif // !OCT_UTILITY