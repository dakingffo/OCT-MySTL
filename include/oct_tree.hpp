#pragma once

#ifndef OCT_TREE
#define OCT_TREE

#include "utility.hpp"

namespace oct {
    struct tree_feature {
        enum class Dir : bool {
            is_left, is_right
        };
    };

    struct red_black_tree_feature : public tree_feature {
        enum class Color : bool {
            black, red
        };
    };

    struct avl_tree_feature : public tree_feature {
        int balance = 0;
    };

    struct splay_tree_feature : public tree_feature {};

    struct treap_featue : public tree_feature {};

    template <typename Ty, typename Feature>
    struct tree_node {};

    template <typename Ty>
    struct tree_node<Ty, red_black_tree_feature> : public red_black_tree_feature {
        using key_type        = key_of_v<Ty>;
        using value_type      = Ty;
        using const_reference = const Ty&;

        using Nodeptr = tree_node*;

        value_type val;
        Nodeptr parent;
        Nodeptr left;
        Nodeptr right;
        Color color;

        tree_node() : tree_node(value_type()) {}

        template <typename Ref>
        tree_node(Ref&& elem, 
                  Nodeptr p = nullptr, 
                  Nodeptr l = nullptr, 
                  Nodeptr r = nullptr, 
                  Color c = Color::red) 
            : val(std::forward<Ref>(elem))
            , parent(p)
            , left(l)
            , right(r)
            , color(c) {}

        tree_node(const tree_node&)            = delete;
        tree_node& operator=(const tree_node&) = delete;
        tree_node(tree_node&&)                 = delete;
        tree_node& operator=(tree_node&&)      = delete;

        Nodeptr Min_node() noexcept {
            Nodeptr node = this;
            while (node->left)
                node = node->left;
            return node;
        }

        Nodeptr Next_node() noexcept {
            if (right) {
                return right->Min_node();
            }
            else {
                Nodeptr node = this;
                while (node->parent && node == node->parent->right)
                    node = node->parent;
                return node->parent ? node->parent : node;
            }
        }

        Nodeptr Max_node() noexcept {
            Nodeptr node = this;
            while (node->right)
                node = node->right;
            return node;
        }

        Nodeptr Prev_node() noexcept {
            if (left) {
                return left->Max_node();
            }
            else {
                Nodeptr node = this;
                while (node->parent && node == node->parent->left)
                    node = node->parent;
                return node->parent ? node->parent : node;
            }
        }

        key_type& Key() noexcept {
            if constexpr (std::is_same_v<key_type, value_type>)
                return val;
            else
                return val.first;
        }

        value_type& Val() noexcept {
            return val;
        }
    };

    template <
        typename Kty,
        typename Vty,
        typename Kcompare,
        typename Vcompare,
        typename Allocator,
        typename Feature
    >
    struct tree_traits_base {
    protected:
        using Alty          = rebind_alloc_t<Allocator, Vty>;
        using Alty_traits   = std::allocator_traits<Alty>;
        using Node          = tree_node<Vty, Feature>;
        using Alnode        = rebind_alloc_t<Allocator, Node>;
        using Alnode_traits = std::allocator_traits<Alnode>;
        using Nodeptr       = typename Alnode_traits::pointer;

    public:
        using key_type           = Kty;
        using value_type         = Vty;
        using size_type          = typename Alty_traits::size_type;
        using difference_type    = typename Alty_traits::difference_type;
        using key_compare        = Kcompare;
        using value_compare      = Vcompare;
        using allocator_type     = Allocator;
        using reference          = Vty&;
        using const_reference    = const Vty&;
        using pointer            = typename Alty_traits::pointer;
        using const_pointer      = typename Alty_traits::const_pointer;
    };

    template <typename Tree>
    struct tree_val {
        using value_type = typename Tree::value_type;
        using size_type  = typename Tree::size_type;

        using Node    = typename Tree::Node;
        using Nodeptr = typename Tree::Nodeptr;

        Node dummy;
        Nodeptr root;
        Nodeptr begin_node;
        size_type val_size;

        tree_val() 
            : dummy()
            , root(nullptr)
            , begin_node(&dummy)
            , val_size(0) {}

        tree_val(tree_val&& another) noexcept {
            operator=(std::move(another));
        }

        tree_val& operator=(tree_val&& another) noexcept {
            if (this == &another)
                return *this;
            dummy.left = another.dummy.left;
            dummy.right = another.dummy.right;
            root = another.root;
            if (root)
                root->parent = &dummy;
            begin_node = another.begin_node;
            val_size = another.val_size;

            another.dummy.left = dummy.right = nullptr;
            another.root = nullptr;
            another.begin_node = &another.dummy;
            another.val_size = 0;
            return *this;
        }
    };

#define OCT_TREE_VAL_PACK_UNWRAP Node&        dummy      = val_pack.second.second.dummy;      \
                                 Nodeptr&     root       = val_pack.second.second.root;       \
                                 Nodeptr&     begin_node = val_pack.second.second.begin_node; \
                                 key_compare& kcomp      = val_pack.get_first();              \
                                 size_type&   val_size   = val_pack.second.second.val_size;   \
                                 Alnode&      alloc      = val_pack.second.get_first();
                                 
    template <
        typename Kty,
        typename Vty,
        typename Kcompare,
        typename Vcompare,
        typename Allocator
    >
    class red_black_tree : public tree_traits_base<Kty, Vty, Kcompare, Vcompare, Allocator, red_black_tree_feature>,
                           public red_black_tree_feature
    {
    protected:
        using Base          = tree_traits_base;
        using Alty          = typename Base::Alty;
        using Alty_traits   = typename Base::Alty_traits;
        using Node          = typename Base::Node;
        using Alnode        = typename Base::Alnode;
        using Alnode_traits = typename Base::Alnode_traits;
        using Nodeptr       = typename Base::Nodeptr;

    public:
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

        using iterator           = tree_iterator<red_black_tree>;
        using node_type          = Node;
        using insert_return_type = pair<iterator, bool>;

    private:
        friend class iterator;
        friend class tree_val<red_black_tree>;

        mutable compressed_pair<key_compare, 
                                compressed_pair<Alnode, tree_val<red_black_tree>>
                               > val_pack;

    protected:
        red_black_tree() : val_pack(key_compare(), Alnode()) {}

        explicit red_black_tree(const key_compare& comp, const allocator_type& alloc = allocator_type())
            : val_pack(comp, alloc) {}

        explicit red_black_tree(const allocator_type& alloc)
            : val_pack(key_compare(), alloc) {}

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        red_black_tree(ForwardIt begin, ForwardIt end,
            const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
            : red_black_tree(comp, alloc) {
            Insert(begin, end);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        red_black_tree(ForwardIt begin, ForwardIt end,
            const allocator_type&)
            : red_black_tree(alloc) {
            Insert(begin, end);
        }

        red_black_tree(const red_black_tree& another, const allocator_type& alloc = allocator_type())
            : red_black_tree(another.Begin(), another.End()) {}

        red_black_tree(red_black_tree&& another, const allocator_type& alloc = allocator_type()) noexcept 
            : val_pack(std::move(another.val_pack)) {}

        red_black_tree(std::initializer_list<value_type> ini_list,
            const key_compare& comp = key_compare(),
            const allocator_type& alloc = allocator_type())
            : red_black_tree(ini_list.begin(), ini_list.end(), comp, alloc) {}

        red_black_tree(std::initializer_list<value_type> ini_list,
            const allocator_type& alloc)
            : red_black_tree(ini_list.begin(), ini_list.end(), alloc) {}

        ~red_black_tree() {
            OCT_TREE_VAL_PACK_UNWRAP
            Tidy(root);

            dummy.left = dummy.right = nullptr;
            root = nullptr;
            begin_node = &dummy;
            val_size = 0;
        }

        red_black_tree& operator=(const red_black_tree& another) {
            if (this == &another)
                return *this;
            *this = red_black_tree(another);
            return *this;
        }

        red_black_tree& operator=(red_black_tree&& another) noexcept {
            if (this == &another)
                return *this;
            this->~red_black_tree();
            val_pack.second.second = std::move(another.val_pack.second.second);
            return *this;
        }

        allocator_type Get_allocator() const noexcept {
            return val_pack.second.Get_first();
        }

        reference operator[](const key_type& key) {
            OCT_TREE_VAL_PACK_UNWRAP

            Nodeptr  parent = &val_pack.second.second.dummy;
            Nodeptr* address = &root;
            while (*address) {
                parent = *address;
                if (kcomp(key, parent->Key())) {
                    address = &parent->left;
                    continue;
                }
                if (kcomp(parent->Key(), key)) {
                    address = &parent->right;
                    continue;
                }
                return (*address)->Val();
            }

            *address = Alnode_traits::allocate(alloc, 1);
            Alnode_traits::construct(alloc, *address, value_type{ key, value_of_v<value_type>() }, parent);
            Nodeptr res = *address;
            Fix_violation_after_insert(*address);

            if (begin_node == &dummy || kcomp(key, begin_node->Key()))
                begin_node = res;
            dummy.left = dummy.right = root;
            val_size++;
            return res->Val();
        }

        iterator Begin() const noexcept {
            return iterator(val_pack.second.second.begin_node);
        }

        iterator End() const noexcept {
            return iterator(&val_pack.second.second.dummy);
        }

        bool Empty() const noexcept {
            return !val_pack.second.second.val_size;
        }

        size_type Size() const noexcept {
            return val_pack.second.second.val_size;
        }

        void Clear() {
            OCT_TREE_VAL_PACK_UNWRAP
            Tidy(root);

            dummy.left = dummy.right = nullptr;
            root = nullptr;
            begin_node = &dummy;
            val_size = 0;
        }

        insert_return_type Insert(const_reference target) {
            OCT_TREE_VAL_PACK_UNWRAP

            Nodeptr  parent = &val_pack.second.second.dummy;
            Nodeptr* address = &root;
            const key_type& target_key = Key_of(target);
            while (*address) {
                parent = *address;
                if (kcomp(target_key, parent->Key())) {
                    address = &parent->left;
                    continue;
                }
                if (kcomp(parent->Key(), target_key)) {
                    address = &parent->right;
                    continue;
                }
                return insert_return_type(*address, false);
            }

            *address = Alnode_traits::allocate(alloc, 1);
            Alnode_traits::construct(alloc, *address, target, parent);
            Nodeptr res = *address;
            Fix_violation_after_insert(*address);

            if (begin_node == &dummy || kcomp(target_key, begin_node->Key()))
                begin_node = res;
            dummy.left = dummy.right = root;
            val_size++;
            return insert_return_type(res, true);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        void Insert(ForwardIt begin, ForwardIt end) {
            while (begin != end)
                Insert(*begin++);
        }

        void Insert(std::initializer_list<value_type> ini_list) {
            Insert(ini_list.begin(), ini_list.end());
        }

        iterator Erase(iterator pos) {
            OCT_TREE_VAL_PACK_UNWRAP
            if (!pos.ptr)
                Iterator_error();
            Nodeptr node = pos.ptr;
            if (begin_node == node)                 //begin++
                begin_node = begin_node->Next_node();
            iterator res(Fix_violation_before_erase(node));
            if (node == node->parent->left)
                node->parent->left = nullptr;
            else if (node == node->parent->right)
                node->parent->right = nullptr;
            Destroy(node);
            if (!(--val_size)) {
                dummy.left = dummy.right = nullptr;
                root = nullptr;
                begin_node = &dummy;
            }
            return res;
        }

        iterator Erase(iterator begin, iterator end) {
            if (!begin.ptr || !end.ptr)
                Iterator_error();
            iterator cur = begin, next = begin;
            for (; cur != end; cur = next) {
                next++;
                Erase(cur);
            }
            return cur;
        }

        template <typename Ref>
        size_type Erase(Ref&& key) {
            iterator pos = Find(std::forward<Ref>(key));
            if (pos != End()) {
                Erase(pos);
                return 1;
            }
            else {
                return 0;
            }
        }

        void Swap(red_black_tree& another) noexcept {
            OCT_TREE_VAL_PACK_UNWRAP
                Nodeptr temp_root = root;
            Nodeptr temp_begin_node = begin_node;
            size_type temp_size = val_size;

            root = another.val_pack.second.second.root;
            dummy.left = dummy.right = root;
            root->parent = &dummy;
            begin_node = another.val_pack.second.second.begin_node;
            val_size = another.val_pack.second.second.val_size;

            another.val_pack.second.second.dummy.left = another.val_pack.second.second.dummy.right = temp_root;
            temp_root->parent = &another.val_pack.second.second.dummy;
            another.val_pack.second.second.root = temp_root;
            another.val_pack.second.second.begin_node = temp_begin_node;
            another.val_pack.second.second.val_size = temp_size;
        }

        template <typename Ref>
        size_type Count(Ref&& key) {
            return Find(std::forward<Ref>(key)) != End();
        }

        template <typename Ref>
        iterator Find(Ref&& key) {
            OCT_TREE_VAL_PACK_UNWRAP
            Nodeptr cur = root;
            while (cur) {
                if (kcomp(key, cur->Key())) {
                    cur = cur->left;
                    continue;
                }
                if (kcomp(cur->Key(), key)) {
                    cur = cur->right;
                    continue;
                }
                break;
            }
            return cur ? iterator(cur) : End();
        }

        template <typename Ref>
        pair<iterator, iterator> Equal_range(Ref&& key) {
            return pair<iterator, iterator>(Lower_bound(key), Upper_bound(key));
        }

        template <typename Ref>
        iterator Lower_bound(Ref&& key) {
            OCT_TREE_VAL_PACK_UNWRAP
            Nodeptr cur = root, pre = nullptr;
            while (cur) {
                if (!kcomp(cur->Key(), key) && !kcomp(key, cur->Key()))
                    return iterator(cur);

                if (kcomp(key, cur->Key())) {
                    pre = cur; 
                    cur = cur->left;
                }
                else {
                    cur = cur->right;
                }
            }
            return pre ? iterator(pre) : End();
        }

        template <typename Ref>
        iterator Upper_bound(Ref&& key) {
            OCT_TREE_VAL_PACK_UNWRAP
            Nodeptr cur = root, pre = nullptr;
            while (cur) {
                if (kcomp(key, cur->Key())) {
                    pre = cur;
                    cur = cur->left;
                }
                else {
                    cur = cur->right;
                }
            }
            return pre ? iterator(pre) : End();
        }

        key_compare Key_comp() const noexcept {
            return key_compare();
        }

        value_compare value_comp() const noexcept {
            return value_compare();
        }

    private:
        void Tidy(Nodeptr node) {
            if (!node)
                return;
            Tidy(node->left);
            Tidy(node->right);

            Destroy(node);
        }

        void Fix_violation_after_insert(Nodeptr node) {
            while (true) {
                Nodeptr parent = node->parent;
                
                if (parent == &val_pack.second.second.dummy) {
                    node->color = Color::black;
                    return;
                }
                else if (!(node->color == Color::red && parent->color == Color::red))
                    return;

                Nodeptr grandparent = parent->parent;
                Dir parent_dir = parent == grandparent->left ? Dir::is_left : Dir::is_right;
                Dir node_dir = node == parent->left ? Dir::is_left : Dir::is_right;
                Nodeptr uncle = (parent_dir == Dir::is_left ? grandparent->right : grandparent->left);

                if (uncle && uncle->color == Color::red) {
                    parent->color = uncle->color = Color::black;
                    grandparent->color = Color::red;
                    node = grandparent;
                }
                else {
                     if (node_dir == Dir::is_left && parent_dir == Dir::is_left) {
                            //LL
                        Rotate_right(grandparent);
                        swap(parent->color, grandparent->color);
                     }
                     else if (node_dir == Dir::is_right && parent_dir == Dir::is_right) {
                            //RR
                        Rotate_left(grandparent);
                        swap(parent->color, grandparent->color);
                     }
                     else if (node_dir == Dir::is_right && parent_dir == Dir::is_left) {
                            //LR
                        Rotate_left(parent);
                        Rotate_right(grandparent);
                        swap(node->color, grandparent->color);
                     }
                     else if (node_dir == Dir::is_left && parent_dir == Dir::is_right) {
                            //RL
                        Rotate_right(parent);
                        Rotate_left(grandparent);
                        swap(node->color, grandparent->color);
                     }
                }
            }
        }

        Nodeptr Fix_violation_before_erase(Nodeptr node) {
            OCT_TREE_VAL_PACK_UNWRAP
            Nodeptr lchild = node->left, rchild = node->right;
            Nodeptr parent = node->parent;
            Nodeptr res = node->Next_node();

            if (!lchild && !rchild) {                   //无孩子
                if (root == node) {                     //根节点直接删
                    New_root(nullptr);
                }
                else if (node->color == Color::black) { //这是非根黑节点
                    Childless_black_erase(node);
                }
                //红节点直接删
            }
            else if (!(lchild && rchild)) {             //一个孩子
                Nodeptr child = lchild ? lchild : rchild;
                child->color = Color::black;            //染色
                New_child(node, child);                 //顶替
            }
            else {                                      //两个孩子
                Nodeptr next = node->Next_node();
                bool node_is_root = node == root;
                bool next_is_root = next == root;
                if (next->parent != node && next != parent) {
                    //交换父节点
                    if (next_is_root)
                        New_root(node);
                    else {
                        if (next == next->parent->left)
                            next->parent->left = node;
                        else
                            next->parent->right = node;
                        node->parent = next->parent;
                    }
                    if (node_is_root)
                        New_root(next);
                    else {
                        if (parent->left == node)
                            parent->left = next;
                        else
                            parent->right = next;
                        next->parent = parent;
                    }
                    //
                    if (next->left)
                        swap(next->left->parent, lchild->parent);
                    else
                        lchild->parent = next;

                    if (next->right)
                        swap(next->right->parent, rchild->parent);
                    else
                        rchild->parent = next;

                    swap(next->left, node->left);
                    swap(next->right, node->right);
                }
                else if (node == next->parent){
                    //只能是   [node]
                    //       []     [next]
                    //           [null]  []

                    /*
                    if (node_is_root)
                        New_root(next);
                    else {
                        if (node == node->parent->left)
                            node->parent->left = next;
                        else
                            node->parent->right = next;
                        next->parent = node->parent;
                    }*/
                    New_child(node, next);
                    node->parent = next;
                    
                    if (node->left)
                        node->left->parent = next;
                    swap(node->left, next->left);
                    if (next->right)
                        next->right->parent = node;
                    node->right = next->right;
                    next->right = node;
                }
                else {
                    //只能是       [next]
                    //        [node]    []
                    //       []  [null]

                    /*
                    if (next == root)
                        New_root(next);
                    else
                        node->parent = next->parent;
                    next->parent = node;
                    */
                    New_child(next, node);
                    if (next->right)
                        next->left->parent = node;
                    swap(node->right, next->right);
                    if (node->left)
                        node->left->parent = next;
                    next->left = node->left;
                    node->left = next;
                }

                swap(next->color, node->color);
                
                Fix_violation_before_erase(node);
            }
            return res;
        }

        void Childless_black_erase(Nodeptr node) {
            OCT_TREE_VAL_PACK_UNWRAP
            Nodeptr parent = node->parent;
            Dir node_dir = node == parent->left ? Dir::is_left : Dir::is_right;
            Nodeptr brother = nullptr;
            Dir brother_dir;
            if (node_dir == Dir::is_left) {
                brother = parent->right;
                parent->left = nullptr;
                brother_dir = Dir::is_right;
            }
            else {
                brother = parent->left;
                parent->right = nullptr; 
                brother_dir = Dir::is_left;
            }

            Nodeptr brother_left = brother ? brother->left : nullptr;
            Nodeptr brother_right = brother ? brother->right : nullptr;

            if (!brother || brother->color == Color::black) {   //兄弟是黑节点
                if (brother_left && brother_left->color == Color::red ||
                    brother_right && brother_right->color == Color::red) {//有红侄子
                    if (brother_dir == Dir::is_left) {              //L
                        if (brother_left && brother_left->color == Color::red) {    //LL
                            brother_left->color = brother->color;
                            brother->color = parent->color;
                            parent->color = Color::black;
                            Rotate_right(parent);
                        }
                        else if (brother_right && brother_right->color == Color::red) {//LR
                            brother_right->color = parent->color;
                            parent->color = Color::black;
                            Rotate_left(brother);
                            Rotate_right(parent);
                        }
                    }
                    else {                                          //R
                        if (brother_right && brother_right->color == Color::red) {    //RR
                            brother_right->color = brother->color;
                            brother->color = parent->color;
                            parent->color = Color::black;
                            Rotate_left(parent);
                        }
                        else if (brother_left && brother_left->color == Color::red) {//RL
                            brother_left->color = parent->color;
                            brother->color = Color::black;
                            Rotate_right(brother);
                            Rotate_left(parent);
                        }
                    }
                }
                else {                                              //兄弟全家黑
                    if (parent->color == Color::red) {
                        if (brother)
                            brother->color = Color::red;
                        parent->color = Color::black;
                    }
                    else {
                        Childless_black_erase(parent);
                    }
                }
            }
            else {                                                  //兄弟红
                brother->color = Color::black;
                parent->color = Color::red;
                if (node_dir == Dir::is_left) {
                    Rotate_left(parent);
                }
                else {
                    Rotate_right(parent);
                }
                Childless_black_erase(node);
            }
        }

        void Destroy(Nodeptr node) {
            Alnode_traits::destroy(val_pack.second.get_first(), node);
            Alnode_traits::deallocate(val_pack.second.get_first(), node, 1);
        }

        void Rotate_left(Nodeptr node) {
            /*             [root]                               [root]
            *          [[5]]    ...                          [10]     ...    
            *       [2]   [10]                   ->      [[5]]  [11]
            *           [8]  [11]                      [2]   [8]
            */
            Nodeptr right = node->right;
            Nodeptr right_left = right->left;
            if (node->parent != &val_pack.second.second.dummy) {
                if (node == node->parent->left)
                    node->parent->left = node->right;
                if (node == node->parent->right)
                    node->parent->right = node->right;
            }
            else {
                val_pack.second.second.root = right;
                val_pack.second.second.dummy.left = val_pack.second.second.dummy.right = right;
            }
            node->right->parent = node->parent;
            node->parent = right;
            right->left = node;
            node->right = right_left;
            if (right_left)
                right_left->parent = node;
        }

        void Rotate_right(Nodeptr node) {
            Nodeptr left = node->left;
            Nodeptr left_right = left->right;
            if (node->parent != &val_pack.second.second.dummy) {
                if (node == node->parent->left) {
                    node->parent->left = node->left;
                }
                if (node == node->parent->right)
                    node->parent->right = node->left;
            }
            else {
                val_pack.second.second.root = left;
                val_pack.second.second.dummy.left = val_pack.second.second.dummy.right = left;
            }
            node->left->parent = node->parent;
            node->parent = left;
            left->right = node;
            node->left = left_right;
            if (left_right)
                left_right->parent = node;
        }

        inline const key_type& Key_of(const_reference val) {
            if constexpr (std::is_same_v<key_type, value_type>)
                return val;
            else
                return val.first;
        }

        void New_root(Nodeptr node) {
            OCT_TREE_VAL_PACK_UNWRAP
            dummy.left = dummy.right = root = node;
            if (node)
                node->parent = &dummy;
        }

        void New_child(Nodeptr node, Nodeptr new_node) {
            if (node == val_pack.second.second.root) {
                New_root(new_node);
            }
            else {
                Nodeptr parent = node->parent;
                if (node == parent->left)
                    parent->left = new_node;
                else
                    parent->right = new_node;
                if (new_node)
                    new_node->parent = parent;
            }
        }

        [[noreturn]] static void Iterator_error() {
            throw std::out_of_range("invalid map iterator");
        }
    };

}

#endif