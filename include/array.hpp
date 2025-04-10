#pragma once

#ifndef OCT_ARRAY
#define OCT_ARRAY

#include <tuple>
#include "type_traits.hpp"

namespace oct {
/*
* characteristics:
* [array<Ty, 0> specialization]
* [CTAD]
* [structured_binding]
*/
    template<typename Ty, std::size_t N>
    struct array {
        static_assert(std::is_object_v<Ty>, "array requires object types");

        using value_type      = Ty;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = Ty*;
        using iterator        = continuous_memory_iterator<array>;

        mutable value_type val[N];

        reference at(size_type pos) const {
            if (pos >= N)
                Subscript_error()
            return val[pos];
        }

        reference operator[](size_type pos) const {
            return val[pos];
        }

        reference front() const {
            return val[0];
        }

        reference back() const {
            return val[N - 1];
        }

        pointer data() const noexcept {
            return val;
        }

        iterator begin() const noexcept {
            return iterator(val);
        }

        iterator end() const noexcept {
            return iterator(val + N);
        }

        bool empty() const noexcept {
            return false;
        }

        size_type size() const noexcept {
            return N;
        }

        size_type max_size() const noexcept {
            return N;
        }

        void fill(const_reference target) {
            std::fill(val, val + N, target);
        }

        void swap(array& another) noexcept {
            for (difference_type i = 0; i < N; i++)
                std::swap(val[i], another.val[i]);
        }

    private:
        [[noreturn]] static void Subscript_error() {
            std::out_of_range("invalid array subscript");
        }
    };

    template<typename Ty>
    struct array<Ty, 0> {   //零长数组偏特化
        static_assert(std::is_object_v<Ty>, "array requires object types");

        using value_type      = Ty;
        using size_type       = std::size_t;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = Ty*;
        using iterator        = continuous_memory_iterator<array>;

        reference at(size_type pos)const {
            Subscript_error();
            return val[pos];
        }

        reference operator[](size_type pos)const {
            return *nullptr;
        }

        reference front() const {
            return *nullptr;
        }

        reference back() const {
            return *nullptr;
        }

        pointer data() const noexcept {
            return nullptr;
        }

        iterator begin() const noexcept {
            return iterator();
        }

        iterator end() const noexcept {
            return iterator();
        }

        bool empty() const noexcept {
            return true;
        }

        size_type size() const noexcept {
            return 0;
        }

        size_type max_size() const noexcept {
            return 0;
        }

        void fill(const_reference target) {}

        void swap(array& another) noexcept {}

    private:
        [[noreturn]] static void Subscript_error() {
            std::out_of_range("invalid array subscript");
        }
    };

    template <typename First, class...Left>  //模版推导设施
    array(First, Left...) -> array<std::enable_if_t<(std::is_same_v<First, Left>&&...), First>, sizeof...(Left) + std::size_t(1)>;

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator==(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        if (!std::is_same_v<Ty1, Ty2> || N1 != N2)
            return false;
        else for (std::size_t i = 0; i < N; i++)
            if (!(left[i] == right[i]))
                return false;
        return true;
    }

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator!=(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        return !(left == right);
    }

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator<(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        for (std::size_t i = 0; i < N1 && i < N2; i++) 
            if (left[i] == right[i])
                continue;
            else return left[i] < right[i];
        return N1 < N2;
    }

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator<=(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        for (std::size_t i = 0; i < N1 && i < N2; i++)
            if (left[i] == right[i])
                continue;
            else return left[i] < right[i];
        return N1 <= N2;
    }

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator>(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        return !(left <= right);
    }

    template <typename Ty1, std::size_t N1, typename Ty2, std::size_t N2>
    bool operator>=(const array<Ty1, N1>& left, const array<Ty2, N2>& right) noexcept {
        return !(left < right);
    }
}

#ifndef OCT_ARRAY_STRUCTURED_BINDING
#define OCT_ARRAY_STRUCTURED_BINDING

namespace std {
    //结构化绑定设施
    template<typename Ty, size_t N>
    struct tuple_size<oct::array<Ty, N>> :
        integral_constant<size_t, N> {};
    
    template<size_t Index, typename Ty, size_t N>
    struct tuple_element<Index, oct::array<Ty, N>> {
        using type = Ty;
    };

    template <size_t Idx, class Ty, size_t N>
    constexpr Ty& get(oct::array<Ty, N>& arr) noexcept {
        return arr[Idx];
    };

    template <size_t Idx, class Ty, size_t N>
    constexpr const Ty& get(const oct::array<Ty, N>& arr) noexcept {
        return arr[Idx];
    };

    template <size_t Idx, class Ty, size_t N>
    constexpr Ty&& get(oct::array<Ty, N>&& arr) noexcept {
        return move(arr[Idx]);
    };

    template <size_t Idx, class Ty, size_t N>
    constexpr const Ty&& get(const oct::array<Ty, N>&& arr) noexcept {
        return move(arr[Idx]);
    };
}

#endif // !OCT_ARRAY_STRUCTURED_BINDING

#endif // !OCT_ARRAY