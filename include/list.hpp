#pragma once

#ifndef OCT_LIST
#define OCT_LIST

#include "utility.hpp"
#include "type_traits.hpp"

namespace oct {
    /*
    * characteristics:
    * [EBO]
    */
    template <typename Ty>
    struct list_node {
        using value_type      = Ty;
        using const_reference = const Ty&;

        using Node    = list_node;
        using Nodeptr = list_node*;

        value_type val;
        Nodeptr next;
        Nodeptr prev;

        Node(const_reference target = value_type(), Nodeptr prev = nullptr, Nodeptr next = nullptr)
            : val(target), prev(prev), next(next) {
            if (!prev && !next)
                this->prev = this->next = this;
        }

        Node(const Node&)            = default;
        Node& operator=(const Node&) = default;

        Node(Node&&) {
            another.next->prev = this;
            another.prev->next = this;
            next = another.next;
            prev = another.prev;
            another.prev = another.next = &another;
        }
        
        Node& operator=(Node&& another) noexcept {
            another.next->prev = this;
            another.prev->next = this;
            next = another.next;
            prev = another.prev;
            another.prev = another.next = &another;
            return *this;
        }
    };

    template <typename List>
    struct list_val {
        using size_type = typename List::size_type;

        using Node    = typename List::Node;
        using Nodeptr = typename List::Nodeptr;

        mutable Node dummy;
        size_type val_size = 0;

        list_val() = default;

        list_val(size_type count) : dummy{}, val_size(count) {}

        list_val(const Node& node, size_type count) : dummy(node), val_size(count) {}

        list_val(Node&& node, size_type count) noexcept
            : dummy(std::move(node)), val_size(count) {
        }
    };

#define OCT_LIST_VAL_PACK_UNWRAP Node&      dummy    = val_pack.second.dummy;    \
                                 size_type& val_size = val_pack.second.val_size; \
                                 Alnode&      alloc  = val_pack.get_first();     \

    template <typename Ty, typename Allocator = allocator<Ty>>
    class list {
    private:    //内部类与内部配置器
        using Alty          = rebind_alloc_t<Allocator, Ty>;
        using Alty_traits   = std::allocator_traits<Alty>;
        using Node          = list_node<Ty>;
        using Alnode        = rebind_alloc_t<Allocator, Node>;
        using Alnode_traits = std::allocator_traits<Alnode>;
        using Nodeptr       = typename Alnode_traits::pointer;

    public:     //统一接口
        static_assert(std::is_object_v<Ty>, "list requires object types");

        using value_type      = Ty;
        using allocator_type  = Allocator;
        using size_type       = typename Alty_traits::size_type;
        using difference_type = typename Alty_traits::difference_type;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = typename Alty_traits::pointer;
        using const_pointer   = typename Alty_traits::const_pointer;
        using iterator        = list_iterator<list>;

    private:    //成员变量
        friend class iterator;
        friend class list_val<list>;

        mutable compressed_pair<Alnode, list_val<list>> val_pack;

    public:     //成员函数
        list() : val_pack(Alnode()) {}

        explicit list(size_type count, const allocator_type& al = allocator_type())
            : list(count, value_type(), al) {}

        explicit list(size_type count, const_reference target, const allocator_type& al = allocator_type())
            : val_pack(al, count) {//提供初始长度和初值的构造
            OCT_LIST_VAL_PACK_UNWRAP
            Insert_n(&dummy, target, count);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        list(ForwardIt begin, ForwardIt end, const allocator_type& al = allocator_type())
            : val_pack(al) {//从迭代器中获取数据进行构造
            OCT_LIST_VAL_PACK_UNWRAP
            val_size = std::distance(begin, end);
            Insert_n(&dummy, begin, end);
        }

        explicit list(const list& another, const allocator_type& al = allocator_type())
            : list(another.begin(), another.end(), al) {}

        explicit list(list&& another, const allocator_type& al = allocator_type()) noexcept
            : val_pack(al, std::move(another.val_pack.second.dummy)
            , another.val_pack.second.val_size) {//移动构造
            another.val_pack.second.val_size = 0;//置空对方，防止对方析构时破坏数据
        }

        list(std::initializer_list<value_type> ini_list, const allocator_type& al = allocator_type())
            : list(ini_list.begin(), ini_list.end(), al) {}

        ~list() {
            OCT_LIST_VAL_PACK_UNWRAP
            Destroy_n(dummy.next, val_size);
            dummy.next = dummy.prev = &dummy;
            val_size = 0;
        }

        list& operator=(const list& another) {//重载拷贝赋值运算符
            if (this == &another)
                return *this;
            *this = list(another);
            return *this;
        }

        list& operator=(list&& another) noexcept {//重载移动赋值运算符
            if (this == &another)
                return *this;
            this->~list();
            OCT_LIST_VAL_PACK_UNWRAP
            dummy = std::move(another.val_pack.second.dummy);
            val_size = another.val_pack.second.val_size;
            another.val_pack.second.val_size = 0;
            return *this;
        }

        void assign(size_type count, const_reference target = value_type()) {//以下是三个再分配的重载
            list temp(count, target);
            swap(temp);
        }

        template <typename ForwardIt>
        void assign(ForwardIt begin, ForwardIt end) {
            list temp(begin, end);
            swap(temp);
        }

        void assign(std::initializer_list<value_type> ini_list) {
            list temp(ini_list);
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

        reference back() const {
            if (!val_pack.second.val_size)
                Logic_error();
            return val_pack.second.dummy.prev->val;
        }

        iterator begin() const noexcept {
            return iterator(val_pack.second.dummy.next);
        }

        iterator end() const noexcept {
            return iterator(&val_pack.second.dummy);
        }

        bool empty() const noexcept {
            return !val_pack.second.val_size;
        }

        size_type size() const noexcept {
            return val_pack.second.val_size;
        }

        void clear() {
            OCT_LIST_VAL_PACK_UNWRAP
            Destroy_n(dummy.next, val_size);
            val_size = 0;
        }

        template <typename Ref>
        iterator insert(iterator it, Ref&& target) {
            if (!it.ptr)
                Iterator_error();
            val_size++;
            return iterator(Insert_n(it->prev, std::forward<Ref>(target), 1));
        }

        iterator insert(iterator it, size_type count, const_reference target) {
            if (!it.ptr)
                Iterator_error();
            val_pack.second.val_size += count;
            return iterator(Insert_n(it->prev, target, count));
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        iterator insert(iterator it, ForwardIt begin, ForwardIt end) {
            if (!it.ptr)
                Iterator_error();
            Nodeptr temp = it->prev;
            Insert_n(temp, begin, end);
            temp = temp->next;
            val_pack.second.val_size += std::distance(begin, end);
            return iterator(it->prev->next);
        }

        iterator insert(iterator it, std::initializer_list<value_type> ini_list) {
            return insert(it, ini_list.begin(), ini_list.end());
        }

        iterator erase(iterator it) {
            if (!it.ptr)
                Iterator_error();
            Nodeptr next = Destroy_n(it.ptr, 1);
            val_pack.second.val_size--;
            return iterator(next);
        }

        iterator erase(iterator begin, iterator end) {
            if (!after_begin.ptr || !(after_begin->next))
                Iterator_error();
            size_type count = std::distance(begin, end);
            Nodeptr next = Destroy_n(it.ptr, count);
            val_pack.second.val_size -= count;
            return iterator(next);
        }

        template <typename Ref>
        void push_back(Ref&& target) {
            OCT_LIST_VAL_PACK_UNWRAP
            Insert_n(dummy.prev, std::forward<Ref>(target), 1);
            val_size++;
        }

        template<typename Ref>
        list& operator<<(Ref&& elem) {//尾部压入元素（似乎重载运算符会更直观，返回*this的引用使得支持arr<<1<<2<<3）
            push_back(std::forward<Ref>(elem));
            return *this;
        }

        void pop_back() {
            OCT_LIST_VAL_PACK_UNWRAP
            Destroy_n(dummy.prev, 1);
            val_size--;
        }

        template <typename Ref>
        void push_front(Ref&& target) {
            OCT_LIST_VAL_PACK_UNWRAP
            Insert_n(&dummy, std::forward<Ref>(target), 1);
            val_size++;
        }

        void pop_front() {
            OCT_LIST_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            Destroy_n(dummy.next, 1);
            val_size--;
        }

        void resize(size_type count, const_reference value = value_type()) {
            OCT_LIST_VAL_PACK_UNWRAP
            if (val_size == count) {
                return;
            }
            else if (val_size < count) {
                Insert_n(dummy.prev, value, count - val_size);
            }
            else {
                Nodeptr last = &dummy;
                for (size_type i = 0; i < count; i++)
                    last = last->next;
                Destroy_n(last, val_size - count);
            }
            val_size = count;
        };

        void swap(list& another) noexcept {
            OCT_LIST_VAL_PACK_UNWRAP

            Nodeptr temp_prev = dummy.prev;
            Nodeptr temp_next = dummy.next;

            another.val_pack.second.dummy.prev->next = &dummy;
            another.val_pack.second.dummy.next->prev = &dummy;
            dummy.next = another.val_pack.second.dummy.next;
            dummy.prev = another.val_pack.second.dummy.prev;

            temp_next->prev = &another.val_pack.second.dummy;
            temp_prev->next = &another.val_pack.second.dummy;
            another.val_pack.second.dummy.prev = temp_prev;
            another.val_pack.second.dummy.next = temp_next;

            size_type temp_size = val_size;
            val_size = another.val_pack.second.val_size;
            another.val_pack.second.val_size = temp_size;
        }

        template <class Compare = less<value_type>>
        void merge(list& another, const Compare& comp = Compare()) {
            iterator A = begin(), B = another.begin();
            for (; A != end() && B != another.end(); A++) {
                if (comp(*A, *B)) {
                    Link_from(A.ptr, (B++).ptr);
                    A++;
                }
            }
            if (B != another.end())
                Link_from(A.ptr->prev, B.ptr, another.end().ptr);
            val_pack.second.val_size += another.val_pack.second.val_size;
            another.val_pack.second.val_size = 0;
        }

        template <class Compare = less<value_type>>
        void merge(list&& another, const Compare& comp = Compare()) {
            merge(another, comp);
        }

        void splice(iterator pos, list& another, iterator begin = another.begin(), iterator end = another.end()) {
            size_type count = std::distance(begin, end);
            Link_from(pos.ptr->prev, begin.ptr, end.ptr);
            val_pack.second.val_size += count;
            another.val_pack.second.val_size -= count;
        }

        void splice_after(iterator pos, list&& another, iterator begin = another.begin(), iterator end = another.end()) {
            splice_after(pos, another, begin, end);
        }
        
        size_type remove(const value_type elem) {
            OCT_LIST_VAL_PACK_UNWRAP
            size_type res = 0;
            for (iterator it = begin(); it != end(); it++) {
                while (it != end() && *it = elem) {
                    it = iterator(Destroy_n(it.ptr, 1));
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        template <class Predicate>
        size_type remove_if(Predicate pr) {
            OCT_LIST_VAL_PACK_UNWRAP
                size_type res = 0;
            for (iterator it = begin(); it != end(); it++) {
                while (it != end() && pr(*it)) {
                    it = iterator(Destroy_n(it.ptr, 1));
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        void reverse() noexcept {
            OCT_LIST_VAL_PACK_UNWRAP
            if (val_size <= 1)
                 return;
            iterator pre = begin(), cur = ++begin();
            while (cur != end()) {
                pre->next = cur->next;
                pre->next->prev = pre.ptr;

                cur->next = dummy.next;
                cur->next->prev = cur.ptr;
                dummy.next = cur.ptr;
                cur->prev = &dummy;

                cur = pre;
                cur++;
            }
        }

        size_type unique() {
            OCT_LIST_VAL_PACK_UNWRAP
            size_type res = 0;
            for (iterator it = begin(); it != end(); it++) {
                while (it->next != &dummy && it->val == it->next->val) {
                    Destroy_n(it->next, 1);
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        template < class BinaryPredicate >
        size_type unique(BinaryPredicate bpr) {
            OCT_LIST_VAL_PACK_UNWRAP
                size_type res = 0;
            for (iterator it = begin(); it != end(); it++) {
                while (it->next != &dummy && bpr(it->val, it->next->val)) {
                    Destroy_n(it->next, 1);
                    res++;
                }
            }
            val_size -= res;
            return res;
        }

        template <class Compare = less<value_type>>
        void sort(const Compare& comp = Compare()) {
            Node& dummy = val_pack.second.dummy;
            dummy.prev->next = nullptr;
            dummy.prev = nullptr;

            Recursion(begin(), comp);

            iterator it = begin();
            it->prev = &dummy;
            while (it->next) {
                it->next->prev = it.ptr;
                it++;
            }
            it->next = &dummy;
            dummy.prev = it.ptr;
        }
        
    private:
        template <typename Ref>
        Nodeptr Insert_n(Nodeptr pre, Ref&& target, size_type n) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr res = Alnode_traits::allocate(alloc, 1);
            Alnode_traits::construct(alloc, res, std::forward<Ref>(target), pre, pre->next);
            res->prev->next = res;
            res->next->prev = res;

            Nodeptr node = res;
            pre = pre->next;
            while (--n) {
                node = Alnode_traits::allocate(alloc, 1);
                Alnode_traits::construct(alloc, node, std::forward<Ref>(target), pre, pre->next);

                node->prev->next = node;
                node->next->prev = node;

                pre = pre->next;
            }
            return res;
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        Nodeptr Insert_n(Nodeptr pre, ForwardIt begin, ForwardIt end) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr res = Alnode_traits::allocate(alloc, 1);
            Alnode_traits::construct(alloc, res, *begin++, pre, pre->next);
            res->prev->next = res;
            res->next->prev = res;

            Nodeptr node = res;
            pre = pre->next;
            while (begin != end) {
                node = Alnode_traits::allocate(alloc, 1);
                Alnode_traits::construct(alloc, node, *begin++, pre, pre->next);

                node->prev->next = node;
                node->next->prev = node;

                pre = pre->next;
            }
            return res;
        }

        Nodeptr Destroy_n(Nodeptr ptr, size_type n) {
            Alnode& alloc = val_pack.get_first();
            Nodeptr pre = ptr->prev;
            while (n--) {
                pre->next = ptr->next;
                Alnode_traits::destroy(alloc, ptr);
                Alnode_traits::deallocate(alloc, ptr, 1);
                ptr = pre->next;
            }
            ptr->prev = pre;
            return ptr;
        }

        void Link_from(Nodeptr pre, Nodeptr node) {
            node->next->prev = node->prev;
            node->prev->next = node->next;

            node->next = pre->next;
            node->prev = pre;

            node->prev->next = node;
            node->next->prev = node;
        }

        void Link_from(Nodeptr pre, Nodeptr l, Nodeptr r) {
            Nodeptr before_r = r->prev;

            l->prev->next = r;
            r->prev = l->prev;

            before_r->next = pre->next;
            before_r->next->prev = before_r;

            pre->next = l;
            l->prev = pre;
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
                if (comp(*left, *right))
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
            throw std::logic_error("invalid list size");
        }

        [[noreturn]] static void Iterator_error() {
            throw std::out_of_range("invalid list iterator");
        }
    };

    template <typename Ref, typename Ty, typename Allocator>
    list<Ty, Allocator>& operator>>(Ref&& elem, list<Ty, Allocator>& lst) {
        lst.push_front(std::forward<Ref>(elem));
        return lst;
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator==(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
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
    bool operator!=(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
        return !(left == right);
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator<(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() < right.size();
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator<=(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() <= right.size();
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator>(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
        return !(left <= right);
    }

    template <typename Ty1, typename Alloc1, typename Ty2, typename Alloc2>
    bool operator>=(const list<Ty1, Alloc1>& left, const list<Ty2, Alloc2>& right) noexcept {
        return !(left > right);
    }
}

#endif // !OCT_LIST