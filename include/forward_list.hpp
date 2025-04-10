#pragma once

#ifndef OCT_FORWARD_LIST
#define OCT_FORWARD_LIST

#include "utility.hpp"
#include "type_traits.hpp"

namespace oct {
/*
* characteristics:
* [EBO]
*/
    template <typename Ty>
    struct forward_list_node {
        using value_type      = Ty;
        using const_reference = const Ty&;

        using Node    = forward_list_node;
        using Nodeptr = forward_list_node*;

        value_type val;
        Nodeptr next = nullptr;

        Node(const_reference target = value_type()) :val(target) {}

        Node(const Node&)            = default;
        Node(Node&&)                 = default;
        Node& operator=(const Node&) = default;
        Node& operator=(Node&&)      = default;
    };

    template <typename Flist>
    struct forward_list_val {
        using size_type = typename Flist::size_type;

        using Node    = typename Flist::Node;
        using Nodeptr = typename Flist::Nodeptr;

        mutable Node dummy;
        size_type val_size = 0;

        forward_list_val() = default;

        forward_list_val(size_type count) : dummy{}, val_size(count){}

        forward_list_val(const Node& node, size_type count) : dummy(node), val_size(count) {}

        forward_list_val(Node&& node, size_type count) noexcept 
            : dummy(std::move(node)), val_size(count) {}
    };

#define OCT_FORWARD_LIST_VAL_PACK_UNWRAP Node&      dummy    = val_pack.second.dummy;    \
                                         size_type& val_size = val_pack.second.val_size; \
                                         Alnode&    alloc    = val_pack.get_first();

    template <typename Ty, typename Allocator = allocator<Ty>>
    class forward_list {
    private:    //内部类与内部配置器
        using Alty          = rebind_alloc_t<Allocator, Ty>;
        using Alty_traits   = std::allocator_traits<Alty>;
        using Node          = forward_list_node<Ty>;
        using Alnode        = rebind_alloc_t<Allocator, Node>;
        using Alnode_traits = std::allocator_traits<Alnode>;
        using Nodeptr       = typename Alnode_traits::pointer;

    public:     //统一接口
        static_assert(std::is_object_v<Ty>, "forward_list requires object types");

        using value_type      = Ty;
        using allocator_type  = Allocator;
        using size_type       = typename Alty_traits::size_type;
        using difference_type = typename Alty_traits::difference_type;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = typename Alty_traits::pointer;
        using const_pointer   = typename Alty_traits::const_pointer;
        using iterator        = forward_list_iterator<forward_list>;

    private:    //成员变量
        friend class iterator;
        friend class forward_list_val<forward_list>;

        mutable compressed_pair<Alnode, forward_list_val<forward_list>> val_pack;

    public:     //成员函数
        forward_list() : val_pack(Alnode()) {}

        explicit forward_list(const allocator_type& al)
            : val_pack(al) {}

        explicit forward_list(size_type count, const allocator_type& al = allocator_type())
            : forward_list(count, value_type(), al) {}

        explicit forward_list(size_type count, const_reference target, const allocator_type& al = allocator_type())
            : val_pack(al, count) {//提供初始长度和初值的构造
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            Append_n(&dummy, target, count);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        forward_list(ForwardIt begin, ForwardIt end, const allocator_type& al = allocator_type())
            : val_pack(al) {//从迭代器中获取数据进行构造
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            val_size = std::distance(begin, end);
            Append_n(&dummy, begin, val_size);
        }

        explicit forward_list(const forward_list& another, const allocator_type& al = allocator_type())
            : forward_list(another.begin(), another.end(), al) {}

        explicit forward_list(forward_list&& another, const allocator_type& al = allocator_type()) noexcept
            : val_pack(al, std::move(another.val_pack.second.dummy), another.val_pack.second.val_size) {//移动构造
            another.val_pack.second.dummy.next = nullptr;
            another.val_pack.second.val_size = 0;//置空对方，防止对方析构时破坏数据
        }

        forward_list(std::initializer_list<value_type> ini_list, const allocator_type& al = allocator_type())
            : forward_list(ini_list.begin(), ini_list.end(), al) {}

        ~forward_list() {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            Destroy_n(&dummy, val_size);
            val_size = 0;
        }

        forward_list& operator=(const forward_list& another) {//重载拷贝赋值运算符
            if (this == &another)
                return *this;
            *this = forward_list(another);
            return *this;
        }

        forward_list& operator=(forward_list&& another) noexcept {//重载移动赋值运算符
            if (this == &another)
                return *this;
            this->~forward_list();
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP

            dummy = another.val_pack.second.dummy;
            val_size = another.val_pack.second.val_size;

            another.val_pack.second.dummy.next = nullptr;//置空对方，防止对方析构时破坏窃取到的数据
            another.val_pack.second.val_size = 0;
            return *this;
        }

        void assign(size_type count, const_reference target = value_type()) {//以下是三个再分配的重载
            forward_list<value_type, allocator_type>temp(count, target);
            swap(temp);
        }

        template <typename ForwardIt>
        void assign(ForwardIt begin, ForwardIt end) {
            forward_list<value_type, allocator_type>temp(begin, end);
            swap(temp);
        }

        void assign(std::initializer_list<value_type> ini_list) {
            forward_list<value_type, allocator_type>temp(ini_list);
            swap(temp);
        }

        allocator_type get_allocator() noexcept {
            return val_pack.get_first();
        }

        reference front() const {
            if (!val_pack.second.val_size)
                Logic_error();
            return val_pack.second.dummy.next->val;
        }

        iterator before_begin() const noexcept {
            return iterator(&val_pack.second.dummy);
        }

        iterator begin() const noexcept {
            return iterator(val_pack.second.dummy.next);
        }

        iterator end() const noexcept {
            return iterator(nullptr);
        }

        bool empty() const noexcept {
            return !val_pack.second.val_size;
        }

        size_type size() const noexcept {
            return val_pack.second.val_size;
        }

        void clear() {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            Destroy_n(&dummy, val_size);
            val_size = 0;
        }

        template <typename Ref>
        iterator insert_after(iterator it, Ref&& target) {
            if (!it.ptr)
                Iterator_error();
            Nodeptr last = Append_n(it.ptr, std::forward<Ref>(target), 1);
            val_pack.second.val_size++;
            return iterator(last);
        }

        iterator insert_after(iterator it, size_type count, const_reference target) {
            if (!it.ptr)
                Iterator_error();
            Nodeptr last = Append_n(it.ptr, target, count);
            val_pack.second.val_size += count;
            return iterator(last);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        iterator insert_after(iterator it, ForwardIt begin, ForwardIt end) {
            if (!it.ptr)
                Iterator_error();
            size_type count = std::distance(begin, end);
            Nodeptr last = Append_n(it.ptr, begin, count);
            val_pack.second.val_size += count;
            return iterator(last);
        }

        iterator insert_after(iterator it, std::initializer_list<value_type> ini_list) {
            return insert_after(it, ini_list.begin(), ini_list.end());
        }

        iterator erase_after(iterator it) {
            if (!it.ptr || !(it->next))
                Iterator_error();
            Nodeptr next = Destroy_n(it.ptr, 1);
            val_pack.second.val_size--;
            return iterator(next);
        }

        iterator erase_after(iterator after_begin, iterator end) {
            if (!after_begin.ptr || !(after_begin->next))
                Iterator_error();
            size_type count = std::distance(after_begin, end) - 1;
            Nodeptr next = Destroy_n(after_begin, count);
            val_pack.second.val_size -= count;
            return iterator(next);
        }

        template <typename Ref>
        void push_front(Ref&& target) {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            Append_n(&dummy, std::forward<Ref>(target), 1);
            val_size++;
        }

        void pop_front() {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            Destroy_n(&dummy, 1);
            val_size--;
        }

        void resize(size_type count, const_reference value = value_type()) {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            if (val_size == count) {
                return;
            }
            else if (val_size < count) {
                Nodeptr last = &dummy;
                for (size_type i = 0; i < val_size; i++)
                    last = last->next;
                Append_n(last, value, count - val_size);
            }
            else {
                Nodeptr last = &dummy;
                for (size_type i = 0; i < count; i++)
                    last = last->next;
                Destroy_n(last, val_size - count);
            }
            val_size = count;
        };

        void swap(forward_list& another) noexcept {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP

            Node temp_dummy = another.val_pack.second.dummy;
            size_type temp_size = another.val_pack.second.val_size;

            another.val_pack.second.dummy = dummy;
            another.val_pack.second.val_size = val_size;

            dummy = temp_dummy;
            val_size = temp_size;
        }

        template <class Compare = less<value_type>>
        void merge(forward_list& another, const Compare& comp = Compare()) {
            iterator after_merge = before_begin();
            iterator A = begin(), B = another.begin();
            for (; A != end() && B != another.end(); after_merge++) {
                if (comp(A->val, B->val))
                    after_merge->next = (A++).ptr;
                else 
                    after_merge->next = (B++).ptr;
            }
            if (B != another.end())
                after_merge->next = B.ptr;

            val_pack.second.val_size += another.val_pack.second.val_size;
            another.val_pack.second.dummy.next = nullptr;
            another.val_pack.second.val_size = 0;
        }

        template <class Compare = less<value_type>>
        void merge(forward_list&& another, const Compare& comp = Compare()) {
            merge(another, comp);
        }

        void splice_after(iterator pos, forward_list& another, iterator begin = another.before_begin(), iterator end = another.end()) {
            Nodeptr this_next = pos->next, another_next = begin->next;
            size_type count = 0;
            iterator another_it = begin;
            while (another_it->next != end.ptr) {
                count++;
                another_it++;
            }
            begin->next = end.ptr;
            another_it->next = this_next;
            pos->next = another_next;

            val_pack.second.val_size += count;
            another.val_pack.second.val_size -= count;
        }

        void splice_after(iterator pos, forward_list&& another, iterator begin = another.before_begin(), iterator end = another.end()) {
            splice_after(pos, another, begin, end);
        }

        size_type remove(const value_type elem) {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            size_type res = 0;
            Nodeptr ptr = &dummy;
            while (ptr && ptr->next) {
                if (ptr->next->val == elem) {
                    Destroy_n(ptr, 1);
                    res++;
                }
                ptr = ptr->next;
            }
            val_size -= res;
            return res;
        }

        template <class Predicate>
        size_type remove_if(Predicate pr) {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            size_type res = 0;
            for (Nodeptr ptr = &dummy; ptr && ptr->next;) {
                if (pr(ptr->next->val)) {
                    Destroy_n(ptr, 1);
                    res++;
                }
                else 
                    ptr = ptr->next;
            }
            val_size -= res;
            return res;
        }

        void reverse() noexcept {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            if (val_size <= 1)
                return;
            iterator pre = begin(), cur = ++begin();
            while (cur != end()) {
                pre->next = cur->next;
                cur->next = dummy.next;
                dummy.next = cur.ptr;
                cur = pre->next;
            }
        }

        size_type unique() {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
            size_type res = 0;
            for (Nodeptr ptr = dummy.next; ptr && ptr->next; ptr = ptr->next) {
                while (ptr->next && ptr->val == ptr->next->val) {
                    Destroy_n(ptr, 1);
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        template <class BinaryPredicate>
        size_type unique(BinaryPredicate bpr) {
            OCT_FORWARD_LIST_VAL_PACK_UNWRAP
                size_type res = 0;
            for (Nodeptr ptr = dummy.next; ptr && ptr->next; ptr = ptr->next) {
                while (ptr->next && bpr(ptr->val, ptr->next->val)) {
                    Destroy_n(ptr, 1);
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        template <class Compare = less<value_type>>
        void sort(const Compare& comp = Compare()) {
            Recursion(begin(), comp);
        }

    private:
        Nodeptr Append_n(Nodeptr ptr, const_reference target, size_type n) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr temp = ptr->next;
            while (n--) {
                ptr->next = Alnode_traits::allocate(alloc,1);
                Alnode_traits::construct(alloc, ptr->next, target);
                ptr = ptr->next;
            }
            ptr->next = temp;
            return ptr;
        }

        Nodeptr Append_n(Nodeptr ptr, value_type&& target, size_type n = 1) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr temp = ptr->next;
            ptr->next = Alnode_traits::allocate(alloc,1);
            Alnode_traits::construct(alloc, ptr->next, std::move(target));
            ptr->next->next = temp;
            return ptr->next;
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        Nodeptr Append_n(Nodeptr ptr, ForwardIt from, size_type n) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr temp = ptr->next;
            while (n--) {
                ptr->next = Alnode_traits::allocate(alloc,1);
                Alnode_traits::construct(alloc, ptr->next, *from++);
                ptr = ptr->next;
            }
            ptr->next = temp;
            return ptr;
        }

        Nodeptr Destroy_n(Nodeptr ptr, size_type n) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr cur = ptr->next;
            while (cur && n--) {
                Nodeptr next = cur->next;
                Alnode_traits::destroy(alloc, cur);
                Alnode_traits::deallocate(alloc, cur, 1);
                cur = next;
            }
            ptr->next = cur;
            return cur;
        }

        template <class Compare>
        iterator Recursion(iterator head, const Compare& comp) {
            if (!head.ptr || !head->next) 
                return head;
            iterator slow = head, fast = head->next;
            while (fast.ptr && fast->next) {
                slow++;
                fast++, fast++;
            }
            iterator left = head, right(slow->next);
            slow->next = nullptr;
            return Merge(Recursion(left, comp), Recursion(right, comp), comp);
        }

        template <class Compare>
        iterator Merge(iterator left, iterator right, const Compare& comp) {
            Node dummy_node;
            iterator pre(&val_pack.second.dummy);
            for (; left.ptr && right.ptr; pre++)
                if (comp(left->val, right->val)) 
                    pre->next = (left++).ptr;
                else 
                    pre->next = (right++).ptr;
            if (left.ptr) 
                pre->next = left.ptr;
            if (right.ptr) 
                pre->next = right.ptr;
            return val_pack.second.dummy.next;
        }

        [[noreturn]] static void Logic_error() {
            throw std::logic_error("invalid forward_list size");
        }

        [[noreturn]] static void Iterator_error() {
            throw std::out_of_range("invalid forward_list iterator");
        }
    };

    template <typename Ref, typename Ty, typename Allocator>
    forward_list<Ty, Allocator>& operator>>(Ref&& elem, forward_list<Ty, Allocator>& f_list) {
        f_list.push_front(std::forward<Ref>(elem));
        return f_list;
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator==(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        if (!std::is_same_v<Ty1, Ty2> || left.size() != right.size())
            return false;
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (!(*l == *r))
                return false;
        return true;
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator!=(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        return !(left == right);
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator<(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() < right.size();
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator<=(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() <= right.size();
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator>(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        return !(left <= right);
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator>=(const forward_list<Ty1, Alloc1>& left, const forward_list<Ty2, Alloc2>& right) noexcept {
        return !(left > right);
    }
}

#endif // !OCT_FORWARD_LIST