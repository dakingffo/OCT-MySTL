#pragma once

#ifndef OCT_DEQUE
#define OCT_DEQUE

#include "utility.hpp"
#include "type_traits.hpp"

namespace oct {
/*
* characteristics:
* [EBO]
*/
    template <class Deq>
    struct deque_val {
        using value_type      = typename Deq::value_type;
        using size_type       = typename Deq::size_type;
        using difference_type = typename Deq::difference_type;
        using pointer         = typename Deq::pointer;

        using Mapptr              = typename Deq::Mapptr;
        using Map_difference_type = typename Deq::Map_difference_type;

        static constexpr size_type bytes = sizeof(value_type);
        static constexpr size_type buf_size = bytes <= 4 ? 16 : 8;
        static constexpr size_type min_map_size = 6;

        Mapptr    map      = nullptr;
        size_type map_size = 0;
        size_type val_off  = min_map_size / 3 * buf_size;
        size_type val_size = 0;

        deque_val() = default;

        deque_val(size_type count1, size_type count2, size_type count3)
            : map_size(count1), val_off(count2), val_size(count3) {}

        inline size_type first_buf() const noexcept {
            return val_off / buf_size;
        }

        inline size_type last_buf() const noexcept {
            return (val_off  + val_size - 1) / buf_size;
        }

        inline pointer visit(size_type offset) const {
            return map[offset / buf_size] + offset % buf_size;
        }
    };

#define OCT_DEQUE_VAL_PACK_UNWRAP Mapptr&    map      = val_pack.second.map;      \
                                  size_type& map_size = val_pack.second.map_size; \
                                  size_type& val_off  = val_pack.second.val_off;  \
                                  size_type& val_size = val_pack.second.val_size; \
                                  Alty&      alloc    = val_pack.get_first();

	template <typename Ty, typename Allocator = allocator<Ty>>
	class deque {
    private:
        using Alty                 = rebind_alloc_t<Allocator, Ty>;
        using Alty_traits          = std::allocator_traits<Alty>;
        using Alpty                = rebind_alloc_t<Allocator, typename Alty_traits::pointer>;
        using Alpty_traits         = std::allocator_traits<Alpty>;
        using Mapptr               = typename Alpty_traits::pointer;
        using Map_difference_type  = typename std::iterator_traits<Mapptr>::difference_type;

	public:
        static_assert(std::is_object_v<Ty>, "deque requires object types");

        using value_type      = Ty;
        using allocator_type  = Allocator;
        using size_type       = typename Alty_traits::size_type;
        using difference_type = typename Alty_traits::difference_type;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using pointer         = typename Alty_traits::pointer;
        using const_pointer   = typename Alty_traits::const_pointer;
        using iterator        = deque_iterator<deque>;

    private:
        friend class iterator;
        friend class deque_val<deque>;

        static constexpr size_type min_map_size = deque_val<deque>::min_map_size;
        static constexpr size_type buf_size = deque_val<deque>::buf_size;

        mutable compressed_pair<Alty, deque_val<deque>> val_pack;

    public:
        deque() : val_pack(Alty()) {
            Map_reserve(min_map_size);
        };

        explicit deque(const allocator_type& al)
            : val_pack(al) {
            Map_reserve(min_map_size);
        }

        explicit deque(size_type count, const allocator_type& al = allocator_type())
            : deque(count, value_type(), al) {}

        explicit deque(size_type count, const_reference target, const allocator_type& al = allocator_type())
            : val_pack(al) {
            OCT_DEQUE_VAL_PACK_UNWRAP
            Map_reserve(min_map_size * buf_size / 3 > count ? min_map_size : (count / buf_size + 1) * 3);
            val_size = count;
            Construct_n(val_off, target, count);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        deque(ForwardIt begin, ForwardIt end, const allocator_type& al = allocator_type())
            : val_pack(al) {
            OCT_DEQUE_VAL_PACK_UNWRAP
            size_type count = std::distance(begin, end);
            Map_reserve(min_map_size * buf_size / 3 <= count ? min_map_size : (count / buf_size + 1) * 3);
            val_size = count;
            Construct_n(val_off, begin, count);
        }

        explicit deque(const deque& another, const allocator_type& al = allocator_type())
            : val_pack(al) {
            OCT_DEQUE_VAL_PACK_UNWRAP
            Map_reserve(another.val_pack.second.map_size);
            val_off = another.val_pack.second.val_off;
            val_size = another.val_pack.second.val_size;
            Construct_n(val_off, another.begin(), val_size);
        }

        explicit deque(deque&& another, const allocator_type& al = allocator_type()) noexcept
            : val_pack(al) {
            OCT_DEQUE_VAL_PACK_UNWRAP
            map = another.val_pack.second.map;
            map_size = another.val_pack.second.map_size;
            val_off = another.val_pack.second.val_off;
            val_size = another.val_pack.second.val_size;
            another.val_pack.second.map = nullptr;
            another.val_pack.second.map_size = 0;
            another.val_pack.second.val_off = min_map_size / 3 * buf_size;
            another.val_pack.second.val_size = 0;
        }

        deque(std::initializer_list<value_type> ini_list, const allocator_type& al = allocator_type())
            : deque(ini_list.begin(), ini_list.end(), al) {
        }

        ~deque() {
            OCT_DEQUE_VAL_PACK_UNWRAP
            for (size_type offset = val_off; offset < val_size + val_off; offset++)
                Alty_traits::destroy(alloc, Visit(offset));
            for (Map_difference_type i = 0; i < map_size; i++)
                Alty_traits::deallocate(alloc, map[i], buf_size);
            Alpty_traits::deallocate(Get_alpty(), map, map_size);
        }

        deque& operator=(const deque& another) {//重载拷贝赋值运算符
            if (this == &another)
                return *this;
            *this = deque(another);
            return *this;
        }

        deque& operator=(deque&& another) noexcept {//重载移动赋值运算符
            if (this == &another)
                return *this;
            this->~deque();
            OCT_DEQUE_VAL_PACK_UNWRAP
            map = another.val_pack.second.map;
            map_size = another.val_pack.second.map_size;
            val_off = another.val_pack.second.val_off;
            val_size = another.val_pack.second.val_size;
            another.val_pack.second.map = nullptr;
            another.val_pack.second.map_size = 0;
            another.val_pack.second.val_off = min_map_size / 3 * buf_size;
            another.val_pack.second.val_size = 0;
            return *this;
        }

        void assign(size_type count, const_reference target = value_type()) {//以下是三个再分配的重载
            *this = deque(count, target);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        void assign(ForwardIt begin, ForwardIt end) {
            *this = deque(begin, end);
        }

        void assign(std::initializer_list<value_type> ini_list) {
            *this = deque(ini_list);
        }

        allocator_type get_allocator() const noexcept {
            return val_pack.get_first();
        }

        reference at(size_type pos) const {//安全的下标访问
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (pos >= val_size)
                Subscript_error();
            return *Visit(val_pack.second.val_off + pos);
        }

        reference operator[](size_type pos) const {//重载下标访问（包装）
            return *Visit(val_pack.second.val_off + pos);
        }

        reference front() const {//访问头元素
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return *Visit(val_pack.second.val_off);
        }

        reference back() const {//访问尾元素
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return *Visit(val_pack.second.val_off + val_size - 1);
        }

        iterator begin() const noexcept {
            return iterator(this, val_pack.second.val_off);
        }

        iterator end() const noexcept {
            return iterator(this, val_pack.second.val_off + val_pack.second.val_size);
        }

        size_type size() const noexcept {
            return val_pack.second.val_size;
        }

        bool empty() const noexcept {
            return !val_pack.second.val_size;
        }

        void shrink_to_fit() {
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (val_off != -1ll && map_size > min_map_size) {
                size_type shrink_size = Last_buf() - First_buf() + 1;
                Mapptr temp = Alpty_traits::allocate(Get_alpty(), shrink_size);
                memcpy(temp, map + First_buf(), sizeof(pointer) * shrink_size);
                for (Map_difference_type i = 0; i < First_buf(); i++)
                    Alty_traits::deallocate(alloc, map[i], buf_size);
                for (Map_difference_type i = Last_buf() + 1; i < map_size; i++)
                    Alty_traits::deallocate(alloc, map[i], buf_size);
                Alpty_traits::deallocate(Get_alpty(), map, map_size);
                val_off %= buf_size;
                map = temp;
                map_size = shrink_size;
            }
        }

        void clear() {
            OCT_DEQUE_VAL_PACK_UNWRAP
            Destroy_n(val_off, val_size);
            val_off = map_size / 3 * buf_size;
            val_size = 0;
        }

        /*
        template <typename Ref>
        iterator insert(iterator pos, Ref&& value) {
            if (pos.cont_ptr != this)
                Iterator_error();
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (pos == begin()) {
                push_front(std::forward<Ref>(value));
                return begin();
            }
            else if (pos == end()) {
                push_back(std::forward<Ref>(value));
                return end() - 1;
            }    
            size_type pre = pos.offset - val_off, suf = val_size + val_off - pos.offset;
            //            pre                                 suf
            // [val_off, pos.offset - 1]      [pos.offset, val_off + val_size - 1]
            if (val_off) {
                Shift_left(val_off + diff, 1);
                Construct_n(val_off + diff, std::forward<Ref>(value), 1);
                val_off--;
                for (const auto& elem : *this)
                    std::cout << elem << ' ';
                std::cout << '\n';
            }
            else {
                if (val_off + val_size == map_size * buf_size)
                    Map_reserve(map_size * 3);
                Shift_right(val_off + diff, 1);
                Construct_n(val_off + diff, std::forward<Ref>(value), 1);
            }
            val_size++;
            return iterator(this, val_off + diff);
        }

        template <typename Ref>
        iterator insert(iterator pos, size_type count, const_reference value) {
            if (pos.cont_ptr != this)
                Iterator_error();
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (pos == begin()) {
                while(count--)
                    push_front(value);
                return begin();
            }
            else if (pos == end()) {
                while(count--)
                    push_back(value);
                return end() - count;
            }
            size_type diff = pos.offset - val_off;
            if (val_off >= count) {
                Shift_left(val_off + diff, count);
                Construct_n(val_off + diff - count + 1, value, count);
                val_off -= count;
            }
            else {
                if (val_off + val_size + count >= map_size * buf_size)
                    Map_reserve((val_off + val_size + count) * 3);
                Shift_right(val_off + diff, count);
                Construct_n(val_off + diff - count + 1, value, count);
            }
            val_size += count;
            return iterator(this, val_off + diff);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        iterator insert(iterator pos, ForwardIt begin, ForwardIt end) {
            if (pos.cont_ptr != this)
                Iterator_error();
            OCT_DEQUE_VAL_PACK_UNWRAP
            size_type count = std::distance(begin, end);
            if (pos == this->begin()) {
                while (count--)
                    push_front(*begin++);
                return this->begin();
            }
            else if (pos == this->end()) {
                while (count--)
                    push_back(*begin++);
                return this->end() - count;
            }
            size_type diff = pos.offset - val_off;
            
            if (val_off >= count) {
                Shift_left(val_off + diff, count);
                Construct_n(val_off + diff - count + 1, begin, count);
                val_off -= count;
            }
            else {
                if (val_off + val_size + count >= map_size * buf_size)
                    Map_reserve((val_off + val_size + count) * 3);
                Shift_right(val_off + diff, count);
                Construct_n(val_off + diff - count + 1, begin, count);
            }
            val_size += count;
            return iterator(this, val_off + diff);
        }

        iterator insert(iterator pos, std::initializer_list<value_type> ini_list) {
            return insert(pos, ini_list.begin(), ini_list.end());
        }
        */
        template<typename Ref>
        void push_back(Ref&& elem) {//尾部压入元素 万能引用与完美转发保护其左值or右值语义
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!map)
                Map_reserve(min_map_size);
            else if (val_off + val_size == map_size * buf_size)
                Map_reserve(3 * map_size);
            Construct_n(val_off + val_size, std::forward<Ref>(elem), 1);
            val_size++;
        }

        template<typename Ref>
        deque& operator<<(Ref&& elem) {//尾部压入元素（似乎重载运算符会更直观，返回*this的引用使得支持arr<<1<<2<<3）
            push_back(std::forward<Ref>(elem));
            return *this;
        }

        void pop_back() {//尾删元素
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            val_size--;
            Destroy_n(val_off + val_size, 1);
        }

        template<typename Ref>
        void push_front(Ref&& elem) {//尾部压入元素 万能引用与完美转发保护其左值or右值语义
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!map)
                Map_reserve(min_map_size);
            else if (!val_off)
                Map_reserve(3 * map_size);
            if (val_size++)
                Construct_n(--val_off, std::forward<Ref>(elem), 1);
            else 
                Construct_n(val_off, std::forward<Ref>(elem), 1);
        }

        void pop_front() {//头删元素
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            if (--val_size)
                Destroy_n(val_off++, 1);
            else
                Destroy_n(val_off, 1);
        }

        void swap(deque& another) noexcept {//交换成员变量
            if (this == &another)
                return;
            if constexpr (Alty_traits::propagate_on_container_swap::value) {
                compressed_pair<Alty, deque_val<deque>> temp_val_pack = another.val_pack;
                another.val_pack = val_pack;
                val_pack = temp_val_pack;
            }
            else {
                OCT_DEQUE_VAL_PACK_UNWRAP
                Mapptr temp_map = another.val_pack.second.map;
                size_type temp_map_size = another.val_pack.second.map_size;
                size_type temp_val_off = another.val_pack.second.val_off;
                size_type temp_val_size = another.val_pack.second.val_size;
                another.val_pack.second.map = map;
                another.val_pack.second.map_size = map_size;
                another.val_pack.second.val_off = val_off;
                another.val_pack.second.val_size = val_size;
                map = temp_map;
                map_size = temp_map_size;
                val_off = temp_val_off;
                val_size = temp_val_size;
            }
        }

    private:
        void Map_reserve(size_type count) {
            OCT_DEQUE_VAL_PACK_UNWRAP
            if (map) {
                Mapptr temp = Alpty_traits::allocate(Get_alpty(), count);
                memcpy(temp + map_size, map, sizeof(pointer) * map_size);
                Alpty_traits::deallocate(Get_alpty(), map, map_size);
                map = temp;
                for (Map_difference_type i = 0; i < map_size; i++)
                    map[i] = Alty_traits::allocate(alloc, buf_size);
                for (Map_difference_type i = 2 * map_size; i < count; i++)
                    map[i] = Alty_traits::allocate(alloc, buf_size);
                map_size = count;
                val_off = count / 3 * buf_size + (val_off ? val_off : 0);
            }
            else {
                map = Alpty_traits::allocate(Get_alpty(), count);
                map_size = count;
                for (Map_difference_type i = 0; i < map_size; i++)
                    map[i] = Alty_traits::allocate(alloc, buf_size);
                val_off = count / 3 * buf_size;
            }
        }

        void Shift_left(size_type offset, size_type count) {
            // [ele][ele][ele][off][ele](end) -> [ele][ele][ele][off][...count...][ele](end)
            OCT_DEQUE_VAL_PACK_UNWRAP
            for (difference_type i = 0; i <= offset; i++) {
                Construct_n(val_off + i, std::move(*Visit(val_off + count + i)), 1);
                Destroy_n(val_off + count + i, 1);
            }
        }

        void Shift_right(size_type offset, size_type count) {
            // [ele][ele][ele][off][ele](end) -> [ele][ele][ele][...count...][off][ele](end)
            OCT_DEQUE_VAL_PACK_UNWRAP
            for (difference_type i = val_size - 1 ; i >= (offset - val_off); i--) {
                Construct_n(val_off + i + count, std::move(*Visit(val_off + i)), 1);
                Destroy_n(val_off + i, 1);
            }
        }

        void Construct_n(size_type offset, const_reference target, size_type n) {
            Alty& alloc = val_pack.get_first();
            for (difference_type pos = offset; pos < offset + n; pos++)
                Alty_traits::construct(alloc, Visit(pos), target);
        }

        void Construct_n(size_type offset, value_type&& target, size_type n = 1) {
            Alty_traits::construct(val_pack.get_first(), Visit(offset), std::move(target));
        }

        template<typename ForwardIt, typename std::enable_if_t<std::_Is_iterator_v<ForwardIt>, int> = 0>
        void Construct_n(size_type offset, ForwardIt from, size_type n) {
            Alty& alloc = val_pack.get_first();
            for (difference_type pos = offset; pos < offset + n; pos++)
                Alty_traits::construct(alloc, Visit(pos), *from++);
        }

        void Destroy_n(size_type offset, size_type n) {
            Alty& alloc = val_pack.get_first();
            for (difference_type pos = offset; pos < offset + n; pos++)
                Alty_traits::destroy(alloc, Visit(pos));
        }

        inline Alpty Get_alpty() noexcept {
            return Alpty(val_pack.get_first());
        }

        inline size_type First_buf() noexcept {
            return val_pack.second.first_buf();
        }

        inline size_type Last_buf() noexcept {
            return val_pack.second.last_buf();
        }

        inline pointer Visit(size_type offset) const noexcept {
            return val_pack.second.visit(offset);
        }

        [[noreturn]] static void Logic_error() {
            throw std::logic_error("invalid deque size");
        }

        [[noreturn]] static void Subscript_error() {
            throw std::out_of_range("invalid deque subscript");
        }

        [[noreturn]] static void Iterator_error() {
            throw std::out_of_range("invalid deque iterator");
        }
    };

    template <typename Ref, typename Ty, typename Allocator>
    deque<Ty, Allocator>& operator>>(Ref&& elem, deque<Ty, Allocator>& deq) {
        deq.push_front(std::forward<Ref>(elem));
        return deq;
    }
}

#endif // !OCT_DEQUE