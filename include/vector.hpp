#pragma once

#ifndef OCT_VECTOR
#define OCT_VECTOR

#include "utility.hpp"
#include "type_traits.hpp"

namespace oct{
/*
* characteristics:
* [shrink_choice]
* [trivially_allocator]
* [EBO]
*/
    template <typename Vec>
    struct vector_val {
        using pointer   = typename Vec::pointer;
        using size_type = typename Vec::size_type;

        pointer   val          = nullptr;
        size_type val_size     = 0;
        size_type val_capacity = 0;

        vector_val() = default;

        vector_val(size_type count) : val_size(count), val_capacity(count * 2) {}

        vector_val(pointer ptr, size_type size, size_type capacity) noexcept 
            : val(ptr), val_size(size), val_capacity(capacity) {}
    };

#define OCT_VECTOR_VAL_PACK_UNWRAP pointer&   val          = val_pack.second.val;          \
                                   size_type& val_size     = val_pack.second.val_size;     \
                                   size_type& val_capacity = val_pack.second.val_capacity; \
                                   Alty&      alloc        = val_pack.get_first();

    template <
        typename Ty,
        typename Allocator = std::conditional_t<std::is_trivially_copyable_v<Ty>, trivial_allocator<Ty>, allocator<Ty>>,
        typename Shrink = manual_shrink
    >
    class vector {
    private:                //内部配置器
        using Shrink_category = Shrink;                         //自动收缩标签
        using Alty            = rebind_alloc_t<Allocator, Ty>;      //模板参数集成
        using Alty_traits     = std::allocator_traits<Alty>;        //空间配置器萃取器

    public:                 //统一接口
        static_assert(std::is_object_v<Ty>, "vector requires object types");
        static_assert(is_memory_shrink_tag_v<Shrink>, "invalid vector shrink tag");

        using value_type      = Ty;
        using allocator_type  = Allocator;
        using size_type       = typename Alty_traits::size_type;
        using difference_type = typename Alty_traits::difference_type;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = typename Alty_traits::pointer;
        using const_pointer   = typename Alty_traits::const_pointer;
        using iterator        = continuous_memory_iterator<vector>;

    private:    //成员变量
        friend class iterator;

        mutable compressed_pair<Alty, vector_val<vector>> val_pack;

    public:     //成员函数
        vector() : val_pack(Alty()) {}

        explicit vector(const allocator_type& al)
            : val_pack(al) {}

        explicit vector(size_type count, const allocator_type& al = allocator_type())
            : vector(count, value_type(), al) {}

        explicit vector(size_type count, const_reference target, const allocator_type& al = allocator_type())
            : val_pack(al, count) {
            OCT_VECTOR_VAL_PACK_UNWRAP
            val = Alty_traits::allocate(alloc, val_capacity);
            Construct_n(val, target, count);
        }

        vector(const_pointer begin, const_pointer end, const allocator_type& al = allocator_type())
            : val_pack(al , std::distance(begin, end)) {
            if (!begin || !end)
                Pointer_error();
            OCT_VECTOR_VAL_PACK_UNWRAP
            val = Alty_traits::allocate(alloc, val_capacity);
            if constexpr (is_trivial_allocator_v<Alty>)
                alloc.memory_copy(begin, val, val_size);
            else 
                Construct_n(val, begin, val_size);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        vector(ForwardIt begin, ForwardIt end, const allocator_type& al = allocator_type())
            : val_pack(al) {
            OCT_VECTOR_VAL_PACK_UNWRAP
            size_type count = std::distance(begin, end);
            val_size = count;
            val_capacity = count * 2;
            val = Alty_traits::allocate(alloc, val_capacity);
            Construct_n(val, begin, count);
        }

        explicit vector(const vector& another, const allocator_type& al = allocator_type())
            : vector(another.val_pack.second.val, another.val_pack.second.val + another.val_pack.second.val_size, al) {}

        explicit vector(vector&& another, const allocator_type& al = allocator_type()) noexcept
            : val_pack(al, another.val_pack.second.val, another.val_pack.second.val_size, another.val_pack.second.val_capacity) {//移动构造
            another.val_pack.second.val = nullptr;
            another.val_pack.second.val_size = 0;
            another.val_pack.second.val_capacity = 0;
        }

        vector(std::initializer_list<value_type> ini_list, const allocator_type& al = allocator_type())
            : vector(ini_list.begin(), ini_list.end(), al) {}

        ~vector() {
            OCT_VECTOR_VAL_PACK_UNWRAP
            Destroy_n(val, val_size);//依次析构元素
            Alty_traits::deallocate(alloc, val, val_capacity);//调用空间配置器解分配内存
        }

        vector& operator=(const vector& another) {//重载拷贝赋值运算符
            if (this == &another)
                return *this;
            *this = vector(another);
            return *this;
        }

        vector& operator=(vector&& another) noexcept {//重载移动赋值运算符
            if (this == &another)
                return *this;
            this->~vector();
            OCT_VECTOR_VAL_PACK_UNWRAP

            val = another.val_pack.second.val;
            val_size = another.val_pack.second.val_size;
            val_capacity = another.val_pack.second.val_capacity;

            another.val_pack.second.val = nullptr;
            another.val_pack.second.val_size = 0;
            another.val_pack.second.val_capacity = 0;

            return *this;
        }

        void assign(size_type count, const_reference target = value_type()) {//以下是三个再分配的重载
            *this = vector(count, target);
        }

        void assign(const_pointer begin, const_pointer end) {
            *this = vector(begin, end);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        void assign(ForwardIt begin, ForwardIt end) {
            *this = vector(begin, end);
        }

        void assign(std::initializer_list<value_type> ini_list) {
            *this = vector(ini_list);
        }

        allocator_type get_allocator() const noexcept {
            return val_pack.get_first();
        }

        reference at(size_type pos) const {//安全的下标访问
           OCT_VECTOR_VAL_PACK_UNWRAP
            if (pos >= val_size)
                Subscript_error();
            return val[pos];
        }

        reference operator[](size_type pos) const {//重载下标访问（包装）
            return val_pack.second.val[pos];
        }

        reference front() const {//访问头元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return val[0];
        }

        reference back() const {//访问尾元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return val[val_size - 1];
        }

        pointer data() noexcept {//返回底层指针
            return val_pack.second.val;
        }

        iterator begin() const noexcept {
            return iterator(val_pack.second.val);
        }

        iterator end() const noexcept {
            OCT_VECTOR_VAL_PACK_UNWRAP
            return iterator(val + val_size);
        }

        bool empty() const noexcept {//返回是否为空
            return !val_pack.second.val_size;
        }

        size_type size() const noexcept {//返回实际长度
            return val_pack.second.val_size;
        }

        void reserve(size_type capacity) {//延长可用空间
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (capacity <= val_capacity) {
                return;
            }
            if constexpr (is_trivial_allocator_v<Alty>) {
                val = alloc.reallocate(val, capacity);
            }
            else {
                pointer temp = Alty_traits::allocate(alloc, capacity);//分配新地址
                for (difference_type i = 0; i < val_size; i++) {
                    Construct_n(temp + i, std::move(val[i]), 1);
                    Destroy_n(val + i, 1);
                }
                Alty_traits::deallocate(alloc, val, val_capacity);//解分配老地址
                val = temp;
            }
            val_capacity = capacity;
        }

        size_type capacity() const noexcept {//返回可用空间
            return val_pack.second.val_capacity;
        }

        void shrink_to_fit() noexcept {//收缩可用空间
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (val_size <= val_capacity / 5)
                swap(vector<value_type, Shrink_category, allocator_type>(*this));
        }

        void clear() {//清空
            OCT_VECTOR_VAL_PACK_UNWRAP
            Destroy_n(val, val_size);
            val_size = 0;
        }

        template <typename Ref>
        iterator insert(iterator it, Ref&& elem) {//插入元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            difference_type pos = it.ptr - val;
            Shift_right(pos, 1);
            Construct_n(val + pos, std::forward<Ref>(elem), 1);
            val_size++;
            return iterator(val + pos);
        }

        iterator insert(iterator it, size_type count, const_reference target) {
            OCT_VECTOR_VAL_PACK_UNWRAP
            difference_type pos = it.ptr - val;
            Shift_right(pos, count);
            Construct_n(val + pos, target, count);
            val_size += count;
            return iterator(val + pos);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        iterator insert(iterator it, ForwardIt begin, ForwardIt end) {
            OCT_VECTOR_VAL_PACK_UNWRAP
            difference_type pos = it.ptr - val;
            size_type count = std::distance(begin, end);
            Shift_right(pos, count);
            Construct_n(val + pos, begin, count);
            val_size += count;
            return iterator(val + pos);
        }

        iterator insert(iterator it, std::initializer_list<value_type> ini_list) {
            return insert(it, ini_list.begin(), ini_list.end());
        }

        iterator erase(iterator it) {//指定位置删元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            difference_type pos = it.ptr - val;
            if (pos > val_size) {
                Iterator_error();
            }
            if (pos == val_size) {
                pop_back();
                return iterator(val + val_size);
            }
            else {
                Destroy_n(val + pos, 1);
                for (difference_type i = pos; i < val_size - 1; i++) {
                    Construct_n(val + i, std::move(val[i + 1]), 1);
                    Destroy_n(val + i + 1, 1);
                }
                val_size--;
                Try_shrink();
                return iterator(val + pos);
            }
        }

        iterator erase(iterator begin, iterator end) {//指定连续删元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            difference_type pos = begin.ptr - val;
            difference_type count = end.ptr - begin.ptr;
            if (pos > val_size || pos + count > val_size)
                Iterator_error();
            Destroy_n(val + pos, count);
            for (difference_type i = pos; i < pos + count; i++) {
                Construct_n(val + i, std::move(val[i + count]), 1);
                Destroy_n(val + i + count, 1);
            }
            val_size -= count;
            Try_shrink();
            return iterator(val + pos);
        }

        template<typename Ref>
        void push_back(Ref&& elem) {//尾部压入元素 万能引用与完美转发保护其左值or右值语义
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (!(val_capacity - val_size))
                reserve(val_capacity ? val_capacity * 2 : 2);
            Construct_n(val + val_size, std::forward<Ref>(elem), 1);
            val_size++;
        }

        template<typename Ref>
        vector& operator<<(Ref&& elem) {//尾部压入元素（似乎重载运算符会更直观，返回*this的引用使得支持arr<<1<<2<<3）
            push_back(std::forward<Ref>(elem));
            return *this;
        }

        void pop_back() {//尾删元素
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            val_size--;
            Destroy_n(val + val_size, 1);
            Try_shrink();
        }

        void resize(const size_type count, const_reference target = value_type()) {//截断数据或延长数据
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (count <= val_size) {//截断
                pointer del = val;
                Destroy_n(val + count, val_size - count);//析构多余元素
                val_size = count;
                Try_shrink();//试图收缩
            }
            else {//延长
                if (count > val_capacity)
                    reserve(count * 2);
                Construct_n(val + val_size, target, count - val_size);//填充target
                val_size = count;
                val_capacity = count * 2;
            }
        }

        void swap(vector& another) noexcept {//交换成员变量
            if (this == &another) {
                return;
            }
            if constexpr (Alty_traits::propagate_on_container_swap::value) {
                compressed_pair<Alty, vector_val<vector>> temp_val_pack = another.val_pack;
                another.val_pack = val_pack;
                val_pack = temp_val_pack;
            }
            else {
                OCT_VECTOR_VAL_PACK_UNWRAP

                pointer temp_val = another.val_pack.second.val;
                size_type temp_size = another.val_pack.second.val_size;
                size_type temp_capacity = another.val_pack.second.val_capacity;

                another.val_pack.second.val = val;
                another.val_pack.second.val_size = val_size;
                another.val_pack.second.val_capacity = val_capacity;

                val = temp_val;
                val_size = temp_size;
                val_capacity = temp_capacity;
            }
        }

    private:
        inline void Try_shrink() noexcept {
            if constexpr (is_auto_shrink_v<Shrink_category>)
                shrink_to_fit();
        }

        void Shift_right(difference_type pos, size_type count) {
            OCT_VECTOR_VAL_PACK_UNWRAP
            if (pos < 0 || pos > val_size)
                Iterator_error();
            if (val_capacity - val_size < count)
                reserve(val_capacity * 2 >= val_size + count ? val_capacity * 2 : (val_size + count) * 2);

            if constexpr (is_trivial_allocator_v<Alty>) {
                alloc.memory_move(val + pos, val + pos + count, val_size - pos);
            }
            else for (difference_type i = val_size + count - 1; i != pos + count - 1; i--) {
                Construct_n(val + i, std::move(val[i - count]), 1);
                Destroy_n(val + i - count, 1);
            }
        }

        void Construct_n(pointer ptr, const_reference target, size_type n) {
            Alty& alloc = val_pack.get_first();
            if constexpr (is_character_or_byte_or_bool_v<value_type> && is_trivial_allocator_v<Alty>)
                alloc.memory_set(ptr, target, n);
            else for (difference_type pos = 0; pos < n; pos++)
                Alty_traits::construct(alloc, ptr + pos, target);
        }

        void Construct_n(pointer ptr, value_type&& target, size_type n = 1) {
            Alty_traits::construct(val_pack.get_first(), ptr, std::move(target));
        }

        template<typename ForwardIt, typename std::enable_if_t<std::_Is_iterator_v<ForwardIt>, int> = 0>
        void Construct_n(pointer ptr, ForwardIt from, size_type n) {
            Alty& alloc = val_pack.get_first();
            for (difference_type pos = 0; pos < n; pos++)
                Alty_traits::construct(alloc, ptr + pos, *from++);
        }

        void Destroy_n(pointer ptr, size_type n) {
            Alty& alloc = val_pack.get_first();
            for (difference_type pos = 0; pos < n; pos++)
                Alty_traits::destroy(alloc, ptr + pos);
        }

        [[noreturn]] static void Pointer_error() {
            throw std::out_of_range("invalid pointer");
        }

        [[noreturn]] static void Logic_error() {
            throw std::logic_error("invalid vector size");
        }

        [[noreturn]] static void Subscript_error() {
            throw std::out_of_range("invalid vector subscript");
        }

        [[noreturn]] static void Iterator_error() {
            throw std::out_of_range("invalid vector iterator");
        }
    };


    template <typename Ty1, typename Shrink1 ,typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator==(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        if (!std::is_same_v<Ty1, Ty2> || left.size() != right.size())
            return false;
        else for (std::size_t i = 0; i < left.size(); i++)
            if (!(left[i] == right[i]))
                return false;
        return true;
    }

    template <typename Ty1, typename Shrink1, typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator!=(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        return !(left == right);
    }

    template <typename Ty1, typename Shrink1, typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator<(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        for (std::size_t i = 0; i < left.size() && i < right.size(); i++)
            if (left[i] == right[i])
                continue;
            else return left[i] < right[i];
        return left.size() < right.size();
    }

    template <typename Ty1, typename Shrink1, typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator<=(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        for (std::size_t i = 0; i < left.size() && i < right.size(); i++)
            if (left[i] == right[i])
                continue;
            else return left[i] < right[i];
        return left.size() <= right.size();
    }

    template <typename Ty1, typename Shrink1, typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator>(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        return !(left <= right);
    }

    template <typename Ty1, typename Shrink1, typename Alloc1, typename Ty2, typename Shrink2, typename Alloc2>
    bool operator>=(const vector<Ty1, Shrink1, Alloc1>& left, const vector<Ty2, Shrink2, Alloc2>& right) noexcept {
        return !(left < right);
    }
}

#endif // !OCT_VECTOR