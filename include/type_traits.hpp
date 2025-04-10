#pragma once

#ifndef OCT_TYPE_TRAITS
#define OCT_TYPE_TRAITS

#include <type_traits>//使用std = c++17的基本元编程设施
#include "oct_iterator.hpp"
#include "oct_allocator.hpp"

namespace oct{
    class memory_shrink_tag {};//自动收缩选项
    class auto_shrink   : public memory_shrink_tag {};
    class manual_shrink : public memory_shrink_tag {};

    template <typename Tag, typename = void>
    constexpr bool is_memory_shrink_tag_v = false;

    template <typename Tag>

    constexpr bool is_memory_shrink_tag_v<Tag, std::enable_if_t<std::is_base_of_v<memory_shrink_tag, Tag>>> = true;

    template <typename Tag>
    constexpr bool is_auto_shrink_v = std::is_same_v<Tag, auto_shrink>;

#ifndef OCT_ITERATOR_TYPE_TRAITS
#define OCT_ITERATOR_TYPE_TRAITS
    template <typename Iter>
    using iterator_category_of_v = typename std::iterator_traits<Iter>::iterator_category;

    template <typename Iter, typename = void>
    constexpr bool is_iterator_v = false;
    template <typename Iter>
    constexpr bool is_iterator_v<Iter, std::void_t<iterator_category_of_v<Iter>>> = true;

    template <typename Iter, typename = void>
    constexpr bool is_forward_iterator_v = false;
    template <typename Iter>
    constexpr bool is_forward_iterator_v<Iter, std::enable_if_t<std::is_base_of_v<std::forward_iterator_tag, iterator_category_of_v<Iter>>>> = true;

    template <typename Iter, typename = void>
    constexpr bool is_random_access_iterator_v = false;
    template <typename Iter>
    constexpr bool is_random_access_iterator_v<Iter, std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag, iterator_category_of_v<Iter>>>> = true;
#endif
#ifndef OCT_ALLOCATOR_TYPE_TRAITS
#define OCT_ALLOCATOR_TYPE_TRAITS
    template <class Ty>
    constexpr bool is_character_v = false;
    template <>
    constexpr bool is_character_v<char> = true;
    template <>
    constexpr bool is_character_v<signed char> = true;
    template <>
    constexpr bool is_character_v<unsigned char> = true;
    template <>
    constexpr bool is_character_v<wchar_t> = true;
    template <>
    constexpr bool is_character_v<char16_t> = true;
    template <>
    constexpr bool is_character_v<char32_t> = true;

    template <class Ty>
    constexpr bool is_character_or_bool_v = is_character_v<Ty>;
    template <>
    constexpr bool is_character_or_bool_v<bool> = true;

    template <class Ty>
    constexpr bool is_character_or_byte_or_bool_v = is_character_or_bool_v<Ty>;
    template <>
    constexpr bool is_character_or_byte_or_bool_v<std::byte> = true;
    
    template <typename New, typename Alold>
    struct replace_alloc_arg {};

    template <typename New, template <typename, class...> typename Alloc, typename Old, class...Left>
    struct replace_alloc_arg<New, Alloc<Old, Left...>> {
        using type = Alloc<New, Left...>;
    };
    
    template <typename Alloc, typename Ty>
    using rebind_alloc_t = typename replace_alloc_arg<Ty, Alloc>::type;

    template<typename Alloc>
    using allocator_value_type_of_v = typename std::allocator_traits<Alloc>::value_type;

    template <typename Alloc>
    constexpr bool is_trivial_allocator_v = std::is_same_v<Alloc, trivial_allocator<allocator_value_type_of_v<Alloc>>>;
#endif

    template <typename Ty>
    struct less {
        bool operator()(const Ty& t1, const Ty& t2) const noexcept {
            return t1 < t2;
        }
    };

    template <typename Ty>
    struct greater {
        bool operator()(const Ty& t1, const Ty& t2) const noexcept {
            return !(t1 < t2) && !(t1 == t2);
        }
    };
}

#endif // !OCT_TYPE_TRAITS