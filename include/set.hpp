#pragma once

#ifndef OCT_SET
#define OCT_SET

#include "oct_tree.hpp"

namespace oct{
	template <
		typename Ty, 
		class Compare      = less<Ty>,  
		typename Allocator = allocator<Ty>,
		template <typename, typename, typename, typename, typename>
		typename Tree      = red_black_tree
	>
	class set : public Tree<Ty, Ty, Compare, Compare, Allocator>{
    private:
        using Base          = Tree<Ty, Ty, Compare, Compare, Allocator>;
        using Alty          = typename Base::Alty;
        using Alty_traits   = typename Base::Alty_traits;
        using Node          = typename Base::Node;
        using Alnode        = typename Base::Alnode;
        using Alnode_traits = typename Base::Alnode_traits;
        using Nodeptr       = typename Base::Nodeptr;

    public:
        static_assert(std::is_object_v<Ty>, "set requires object types");

        using key_type           = typename Base::key_type;
        using value_type         = typename Base::value_type;
        using size_type          = typename Base::size_type;
        using difference_type    = typename Base::difference_type;
        using key_compare        = typename Base::key_compare;
        using value_compare      = typename Base::value_type;
        using allocator_type     = typename Base::allocator_type;
        using reference          = typename Base::reference;
        using const_reference    = typename Base::const_reference;
        using pointer            = typename Base::pointer;
        using const_pointer      = typename Base::const_pointer;
        using iterator           = typename Base::iterator;
        using node_type          = typename Base::node_type;
        using insert_return_type = typename Base::insert_return_type;

    private:
        friend typename iterator;

    public:
        set() : Base() {}

        explicit set(const key_compare& comp, const allocator_type& alloc = allocator_type())
            : Base(comp, alloc) {}

        explicit set(const allocator_type& alloc)
            : Base(key_compare(), alloc) {}

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        set(ForwardIt begin, ForwardIt end,
            const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
            : Base(begin, end, comp, alloc) {}

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        set(ForwardIt begin, ForwardIt end,
            const allocator_type& alloc)
            : Base(begin, end, alloc) {}

        set(const set& another, const allocator_type& alloc = allocator_type())
            : Base(another.begin(), another.end()) {
        }

        set(set&& another, const allocator_type& alloc = allocator_type()) noexcept 
            : Base(std::move(another)) {}

        set(std::initializer_list<value_type> ini_list,
            const key_compare& comp = key_compare(),
            const allocator_type& alloc = allocator_type())
            : Base(ini_list, comp, alloc) {}

        set(std::initializer_list<value_type> ini_list,
            const allocator_type& alloc)
            : Base(ini_list, alloc) {}

        ~set() {}

        set& operator=(const set& another) {
            if (this == &another)
                return *this;
            Base::operator=(another);
            return *this;
        }

        set& operator=(set&& another) noexcept {
            if (this == &another)
                return *this;
            Base::operator=(std::move(another));
            return *this;
        }

        allocator_type get_allocator() const noexcept {
            return Base::Get_allocator();
        }

        iterator begin() const noexcept {
            return Base::Begin();
        }

        iterator end() const noexcept {
            return Base::End();
        }

        bool empty() const noexcept {
            return Base::Empty();
        }

        size_type size() const noexcept {
            return Base::Size();
        }

        void clear() {
            Base::Clear();
        }

        insert_return_type insert(const_reference target) {
            return Base::Insert(target);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        void insert(ForwardIt begin, ForwardIt end) {
            Base::Insert(begin, end);
        }

        void insert(std::initializer_list<value_type> ini_list) {
            Base::Insert(ini_list);
        }

        iterator erase(iterator pos) {
            return Base::Erase(pos);
        }

        iterator erase(iterator begin, iterator end) {
            return Base::Erase(begin, end);
        }

        template <typename Ref>
        size_type erase(Ref&& key) {
            return Base::Erase(std::forward<Ref>(key));
        }

        void swap(set& another) noexcept {
            Base::Swap(another);
        }

        template <typename Ref>
        size_type count(Ref&& key) {
            return Base::Count(std::forward<Ref>(key));
        }

        template <typename Ref>
        iterator find(Ref&& key) {
            return Base::Find(std::forward<Ref>(key));
        }

        template <typename Ref>
        pair<iterator, iterator> equal_range(Ref&& key) {
            return Base::Equal_range(std::forward<Ref>(key));
        }

        template <typename Ref>
        iterator lower_bound(Ref&& key) {
            return Base::Lower_bound(std::forward<Ref>(key));
        }

        template <typename Ref>
        iterator upper_bound(Ref&& key) {
            return Base::Upper_bound(std::forward<Ref>(key));
        }

        key_compare key_comp() const noexcept {
            return Base::Key_compare();
        }

        value_compare value_comp() const noexcept {
            return Base::Value_compare();
        }
	};

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename>
        typename Tree
    >
    bool operator==(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        if (!std::is_same_v<Ty1, Ty2> || left.size() != right.size())
            return false;
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (!(*l == *r))
                return false;
        return true;
    }

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename>
        typename Tree
    >
    bool operator!=(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        return !(left == right);
    }

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename>
        typename Tree
    >
    bool operator<(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() < right.size();
    }

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename>
        typename Tree
    >
    bool operator<=(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        auto l = left.begin();
        auto r = right.begin();
        for (; l != left.end() && r != right.end(); l++, r++)
            if (*l == *r)
                continue;
            else return *l < *r;
        return left.size() <= right.size();
    }

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename> 
        typename Tree
    >
    bool operator>(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        return !(left <= right);
    }

    template <
        typename Ty,
        class Compare,
        typename Allocator,
        template <typename, typename, typename, typename, typename>
        typename Tree
    >
    bool operator>=(const set<Ty, Compare, Allocator, Tree>& left, const set<Ty, Compare, Allocator, Tree>& right) noexcept {
        return !(left > right);
    }

    template<typename Ty, typename Compare = std::less<Ty>, typename Alloc = oct::allocator<Ty>>
    set(std::initializer_list<Ty>) -> set<Ty, Compare, Alloc>;

    template<typename Ty, typename Compare = std::less<Ty>, typename Alloc = oct::allocator<Ty>>
    set(std::initializer_list<Ty>, Compare, Alloc) -> set<Ty, Compare, Alloc>;

    template<typename Ty, typename Compare = std::less<Ty>, typename Alloc = oct::allocator<Ty>>
    set(std::initializer_list<Ty>, Compare) -> set<Ty, Compare, Alloc>;
}

#endif // !OCT_SET