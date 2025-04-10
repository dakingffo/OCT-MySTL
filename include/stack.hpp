#pragma once

#ifndef OCT_STACK
#define OCT_STACK

#include "vector.hpp"
#include "forward_list.hpp"

namespace oct {
/*
* characteristics:
* [stack<Ty, forward_list> specialization]
*/
    template <typename Ty, typename Container = vector<Ty>>
    class stack {
    public:
        using container_type  = Container;
        using value_type      = typename Container::value_type;
        using size_type       = typename Container::size_type;
        using reference       = typename Container::reference;
        using const_reference = typename Container::const_reference;

    private:
        container_type cont{};

    public:
        stack() = default;

        explicit stack(const container_type& container) : cont(container) {}

        explicit stack(container_type&& container) noexcept : cont(std::move(container)) {}

        stack(const stack& another) : cont(another.cont) {}

        stack(stack&& another) noexcept : cont(std::move(another.cont)) {}

        stack& operator=(const stack& another) {
            if (this == &another)return *this;
            cont = another.cont;
            return *this;
        }

        stack& operator=(stack&& another) noexcept {
            if (this == &another)return *this;
            cont = std::move(another.cont);
            return *this;
        }

        bool empty() const noexcept {
            return cont.empty();
        }

        size_type size() const noexcept {
            return cont.size();
        }

        template<typename Ref>
        void push(Ref&& elem) {
            cont.push_back(std::forward<Ref>(elem));
        }

        template<typename Ref>
        stack& operator<<(Ref&& elem) {
            cont.push_back(std::forward<Ref>(elem));
            return *this;
        }

        void pop() {
            cont.pop_back();
        }

        stack& operator>>(value_type& elem) {
            elem = std::move(cont.back());
            pop();
            return *this;
        }

        const_reference top() const {
            return cont.back();
        }
    };

    template <typename Ty>
    class stack<Ty, forward_list<Ty>> {
    public:
        using container_type  = forward_list<Ty>;
        using value_type      = typename forward_list<Ty>::value_type;
        using size_type       = typename forward_list<Ty>::size_type;
        using reference       = typename forward_list<Ty>::reference;
        using const_reference = typename forward_list<Ty>::const_reference;

    private:
        container_type cont{};

    public:
        stack() = default;

        explicit stack(const container_type& container) : cont(container) {}

        explicit stack(container_type&& container) noexcept : cont(std::move(container)) {}

        stack(const stack& another) : cont(another.cont) {}

        stack(stack&& another) noexcept : cont(std::move(another.cont)) {}

        stack& operator=(const stack& another) {
            if (this == &another)return *this;
            cont = another.cont;
            return *this;
        }

        stack& operator=(stack&& another) noexcept {
            if (this == &another)return *this;
            cont = std::move(another.cont);
            return *this;
        }

        bool empty() const noexcept {
            return cont.empty();
        }

        size_type size() const noexcept {
            return cont.size();
        }

        template<typename Ref>
        void push(Ref&& elem) {
            cont.push_front(std::forward<Ref>(elem));
        }

        template<typename Ref>
        stack& operator<<(Ref&& elem) {
            cont.push_front(std::forward<Ref>(elem));
            return *this;
        }

        void pop() {
            cont.pop_front();
        }

        stack& operator>>(value_type& elem) {
            elem = std::move(cont.front());
            pop();
            return *this;
        }

        const_reference top() const {
            return cont.front();
        }
    };
}

#endif // !OCT_STACK