#pragma once

#ifndef OCT_ITERATOR
#define OCT_ITERATOR

#include <iterator>
#include <type_traits>

namespace oct {

#ifndef OCT_FORWARD_LIST_ITERATOR
#define OCT_FORWARD_LIST_ITERATOR

    template <typename Flist>
    class forward_list_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = typename Flist::value_type;
        using difference_type   = typename Flist::difference_type;
        using pointer           = typename Flist::pointer;
        using reference         = value_type&;

    private:
        friend Flist;
        using Iter    = forward_list_iterator;
        using Nodeptr = typename Flist::Nodeptr;

        Nodeptr ptr;

    public:
        forward_list_iterator(Nodeptr p = nullptr) : ptr(p) {}

        Iter& operator=(const Iter& another) noexcept {
            ptr = another.ptr;
            return *this;
        }

        reference operator*() const {
            return ptr->val;
        }

        Nodeptr operator->() const {
            return ptr;
        }

        Iter& operator++() noexcept {
            ptr = ptr->next;
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter temp = *this;
            ptr = ptr->next;
            return temp;
        }

        bool operator==(const Iter& another) const noexcept {
            return ptr == another.ptr;
        }

        bool operator!=(const Iter& another) const noexcept {
            return ptr != another.ptr;
        }
    };

#endif // !OCT_FORWARD_LIST_ITERATOR

#ifndef OCT_LIST_ITERATOR
#define OCT_LIST_ITERATOR

    template <typename List>
    class list_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = typename List::value_type;
        using difference_type   = typename List::difference_type;
        using pointer           = typename List::pointer;
        using reference         = value_type&;

    private:
        friend List;
        using Iter = list_iterator;
        using Nodeptr = typename List::Nodeptr;

        Nodeptr ptr;

    public:
        list_iterator(Nodeptr p = nullptr) : ptr(p) {}

        Iter& operator=(const Iter& another) noexcept {
            ptr = another.ptr;
            return *this;
        }

        reference operator*() const {
            return ptr->val;
        }

        Nodeptr operator->() const {
            return ptr;
        }

        Iter& operator++() noexcept {
            ptr = ptr->next;
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter temp = *this;
            ptr = ptr->next;
            return temp;
        }

        Iter& operator--() noexcept {
            ptr = ptr->prev;
            return *this;
        }

        Iter operator--(int) noexcept {
            Iter temp = *this;
            ptr = ptr->prev;
            return temp;
        }

        bool operator==(const Iter& another) const noexcept {
            return ptr == another.ptr;
        }

        bool operator!=(const Iter& another) const noexcept {
            return ptr != another.ptr;
        }
    };

#endif // !OCT_LIST_ITERATOR

#ifndef OCT_TREE_ITERATOR
#define OCT_TREE_ITERATOR

    template <typename Tree>
    class tree_iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = typename Tree::value_type;
        using difference_type   = typename Tree::difference_type;
        using pointer           = typename Tree::pointer;
        using reference         = value_type&;

    private:
        friend Tree;
        using Iter    = tree_iterator;
        using Nodeptr = typename Tree::Nodeptr;

        Nodeptr ptr;

    public:
        tree_iterator(Nodeptr ptr = nullptr) : ptr(ptr) {}

        Iter& operator=(const Iter& another) noexcept {
            ptr = another.ptr;
            return *this;
        }

        reference operator*() const {
            return ptr->val;
        }

        Nodeptr operator->() const {
            return ptr;
        }

        Iter& operator++() noexcept {
            ptr = ptr->Next_node();
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter temp = *this;
            ptr = ptr->Next_node();
            return temp;
        }

        Iter& operator--() noexcept {
            ptr = ptr->Prev_node();
            return *this;
        }

        Iter operator--(int) noexcept {
            Iter temp = *this;
            ptr = ptr->Prev_node();
            return temp;
        }

        bool operator==(const Iter& another) const noexcept {
            return ptr == another.ptr;
        }

        bool operator!=(const Iter& another) const noexcept {
            return ptr != another.ptr;
        }
    };

#endif // !OCT_TREE_ITERATOR

#ifndef OCT_DEQUE_ITERATOR
#define OCT_DEQUE_ITERATOR

    template <typename Deq>
    class deque_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = typename Deq::value_type;
        using difference_type   = typename Deq::difference_type;
        using pointer           = typename Deq::pointer;
        using reference         = value_type&;

    private:
        friend Deq;
        using Iter                = deque_iterator;
        using Mapptr              = typename Deq::Mapptr;
        using Deqptr              = const Deq* const;
        using Map_difference_type = typename Deq::Map_difference_type;
        using size_type           = typename Deq::size_type;

        static constexpr size_type buf_size = Deq::buf_size;

        Deqptr cont_ptr;
        size_type offset;

    public:
        deque_iterator(Deqptr cont_ptr, size_type offset = 0) : cont_ptr(cont_ptr), offset(offset) {}

        Iter& operator=(const Iter& another) noexcept {
            if (cont_ptr != another.cont_ptr)
                Logic_error();
            offset = another.offset;
            return *this;
        }

        reference operator*() const noexcept {
            return *(cont_ptr->Visit(offset));
        }

        reference operator[](difference_type n) const noexcept {
            return *(cont_ptr->Visit(offset + n));
        }

        pointer operator->() const noexcept {
            return cont_ptr->Visit(offset);
        }

        Iter& operator++() noexcept {
            ++offset;
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter temp = *this;
            offset++;
            return temp;
        }

        Iter& operator--() noexcept {
            --offset;
            return *this;
        }

        Iter operator--(int) noexcept {
            Iter temp = *this;
            offset--;
            return temp;
        }

        Iter& operator+=(difference_type n) noexcept {
            offset += n;
            return *this;
        }

        Iter& operator-=(difference_type n) noexcept {
            offset -= n;
            return *this;
        }

        Iter operator+(difference_type n) const noexcept {
            return Iter(cont_ptr, offset + n);
        }

        Iter operator-(difference_type n) const noexcept {
            return Iter(cont_ptr, offset - n);
        }

        difference_type operator-(const Iter& another) const noexcept {
            return offset - another.offset;
        }

        bool operator==(const Iter& another) const noexcept {
            return cont_ptr == another.cont_ptr && offset == another.offset;
        }

        bool operator!=(const Iter& another) const noexcept {
            return cont_ptr != another.cont_ptr || offset != another.offset;
        }

        bool operator<(const Iter& another) const noexcept {
            return offset - another.offset < 0;
        }

        bool operator<=(const Iter& another) const noexcept {
            return offset - another.offset <= 0;
        }

        bool operator>(const Iter& another) const noexcept {continuous_memory_iterator
            return offset - another.offset > 0;
        }

        bool operator>=(const Iter& another) const noexcept {
            return offset - another.offset >= 0;
        }

        private:
            [[noreturn]] static void Logic_error() {
                throw std::logic_error("iterator can't from different deque");
            }
    };

#endif // !OCT_DEQUE_ITERATOR

#ifndef OCT_CONTINUOUS_MEMORY_ITERATOR
#define OCT_CONTINUOUS_MEMORY_ITERATOR

    template <typename Container>
    class continuous_memory_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = typename Container::value_type;
        using difference_type   = typename Container::difference_type;
        using pointer           = typename Container::pointer;
        using reference         = value_type&;

    private:
        friend Container;
        using Iter = continuous_memory_iterator;

        pointer ptr;

    public:
        continuous_memory_iterator(pointer p = nullptr) :ptr(p) {}

        Iter& operator=(const Iter& another) noexcept {
            ptr = another.ptr;
            return *this;
        }

        reference operator*() const noexcept {
            return *ptr;
        }

        reference operator[](difference_type n) const noexcept {
            return ptr[n];
        }

        pointer operator->() const noexcept {
            return ptr;
        }

        Iter& operator++() noexcept {
            ++ptr;
            return *this;
        }

        Iter operator++(int) noexcept {
            Iter temp = *this;
            ptr++;
            return temp;
        }

        Iter& operator--() noexcept {
            --ptr;
            return *this;
        }

        Iter operator--(int) noexcept {
            Iter temp = *this;
            ptr--;
            return temp;
        }

        Iter& operator+=(difference_type n) noexcept {
            ptr += n;
            return *this;
        }

        Iter& operator-=(difference_type n) noexcept {
            ptr -= n;
            return *this;
        }

        Iter operator+(difference_type n) const noexcept {
            return Iter(ptr + n);
        }

        Iter operator-(difference_type n) const noexcept {
            return Iter(ptr - n);
        }

        difference_type operator-(const Iter& another) const noexcept {
            return ptr - another.ptr;
        }

        bool operator==(const Iter& another) const noexcept {
            return ptr == another.ptr;
        }

        bool operator!=(const Iter& another) const noexcept {
            return ptr != another.ptr;
        }

        bool operator<(const Iter& another) const noexcept {
            return another.ptr - ptr > 0;
        }

        bool operator<=(const Iter& another) const noexcept {
            return another.ptr - ptr >= 0;
        }

        bool operator>(const Iter& another) const noexcept {
            return another.ptr - ptr < 0;
        }

        bool operator>=(const Iter& another) const noexcept {
            return another.ptr - ptr <= 0;
        }
    };

#endif // !OCT_CONTINUOUS_MEMORY_ITERATOR

}

#endif // !OCT_ITERATOR