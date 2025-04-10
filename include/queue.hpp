#pragma once

#ifndef OCT_QUEUE
#define OCT_QUEUE

#include "deque.hpp"
#include "vector.hpp"

namespace oct {
    template <typename Ty, typename Container = deque<Ty>>
    class queue {
    public:
        using container_type  = Container;
        using value_type      = typename Container::value_type;
        using size_type       = typename Container::size_type;
        using reference       = typename Container::reference;
        using const_reference = typename Container::const_reference;

    private:
        container_type cont{};

    public:
        queue() = default;

        explicit queue(const container_type& container) : cont(container) {}

        explicit queue(container_type&& container) noexcept : cont(std::move(container)) {}

        queue(const queue& another) : cont(another.cont) {}

        queue(queue&& another) noexcept : cont(std::move(another.cont)) {}

        queue& operator=(const queue& another) {
            if (this == &another)return *this;
            cont = another.cont;
            return *this;
        }

        queue& operator=(queue&& another) noexcept {
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

        template <typename Ref>
        void push(Ref&& elem) {
            cont.push_back(std::forward<Ref>(elem));
        }

        template <typename Ref>
        queue& operator<<(Ref&& elem) {
            cont.push_back(std::forward<Ref>(elem));
            return *this;
        }

        template <typename Ty, typename Container>
        friend void operator<<(Ty& elem, queue<Ty, Container>& que);

        void pop() {
            cont.pop_front();
        }

        const_reference front() const {
            return cont.front();
        }

        const_reference back() const {
            return cont.back();
        }
    };

    template <typename Ty, typename Container>
    queue<Ty, Container>& operator<<(Ty& elem, queue<Ty, Container>& que) {
        elem = que.cont.front();
        que.pop();
        return que;
    }

    template <typename Ty, typename Container = vector<Ty>, class Compare = less<typename Container::value_type>>
    class priority_queue {
    public:
        using container_type  = Container;
        using value_type      = typename Container::value_type;
        using size_type       = typename Container::size_type;
        using reference       = typename Container::reference;
        using const_reference = typename Container::const_reference;
        using value_compare   = Compare;

    private:
        compressed_pair<value_compare, container_type> val_pack;

    public:
        priority_queue() {};

        priority_queue(const priority_queue& another) : val_pack.second(another.val_pack.second) {}

        priority_queue(priority_queue&& another) noexcept : val_pack.second(std::move(another.val_pack.second)) {}

        ~priority_queue() {}

        priority_queue& operator=(const priority_queue& another) {
            if (this == &another)return *this;
            ~priority_queue();
            val_pack.second = another.val_pack.second;
            copmare = another.Get_comp();
            return *this;
        }

        priority_queue& operator=(priority_queue&& another) {
            if (this == &another)return *this;
            ~priority_queue();
            val_pack.second = std::move(another.val_pack.second);
            Get_comp() = std::move(another.Get_comp());
            return *this;
        }

        bool empty()const {
            return val_pack.second.empty();
        }

        size_type size()const {
            return val_pack.second.size();
        }

        template<typename V>
        void push(V&& elem) {
            val_pack.second.push_back(elem);
            Up(val_pack.second.size() - 1);
        }

        template<typename V>
        priority_queue& operator<<(V&& elem) {
            val_pack.second.push_back(elem);
            Up(val_pack.second.size() - 1);
            return *this;
        }

        void pop() {
            val_pack.second[0] = val_pack.second.back();
            val_pack.second.pop_back();
            Down(0);
        }

        const_reference top() const {
            return val_pack.second[0];
        }

    private:


        inline size_type Father(size_type pos) const noexcept {
            return (pos - 1) / 2;
        }

        inline size_type Lchild(size_type pos) const noexcept {
            return 2 * pos + 1;
        }

        inline size_type Rchild(size_type pos) const noexcept {
            return 2 * pos + 2;
        }

        inline value_compare& Get_comp() {
            return val_pack.get_first();
        }

        void Up(size_type idx) {
            if (idx && Get_comp()(val_pack.second[Father(idx)], val_pack.second[idx])) {
                std::swap(val_pack.second[Father(idx)], val_pack.second[idx]);
                Up(Father(idx));
            }
        }

        void Down(size_type idx) {
            size_type pos = idx;
            if (Rchild(idx) < val_pack.second.size() && Get_comp()(val_pack.second[pos], val_pack.second[Lchild(idx)]))
                pos = Lchild(idx);
            if (Rchild(idx) < val_pack.second.size() && Get_comp()(val_pack.second[pos], val_pack.second[Rchild(idx)]))
                pos = Rchild(idx);
            if (pos != idx) {
                swap(val_pack.second[idx], val_pack.second[pos]);
                Down(pos);
            }
        }
    };
}

#endif // !OCT_QUEUE