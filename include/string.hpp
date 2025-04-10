#pragma once

#ifndef OCT_STRING
#define OCT_STRING

#include "utility.hpp"
#include "type_traits.hpp"

namespace oct {
/*
* characteristics:
* [char_traits]
* [trivially_allocator]
* [SSO]
* [EBO]
*/

#ifndef OCT_CHAR_TRAITS
#define OCT_CHAR_TRAITS

    template <typename CharT, typename IntT, typename StreamPosT>
    struct char_traits_base {
        using char_type  = CharT;
        using int_type   = IntT;
        using pos_type   = StreamPosT;
        using off_type   = std::streamoff;
        using state_type = std::mbstate_t;
    };
    
    template <typename CharT, typename IntT, typename StreamPosT>
    struct char8or32_traits : public char_traits_base<CharT, IntT, StreamPosT> {
        static void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        };
        static char_type* assign(char_type* ptr, std::size_t count, char_type c2) noexcept {
            if constexpr (sizeof(char_type) == 1)
                return reinterpret_cast<char_type*>(memset(ptr, c2, count));
            else {
                for (std::size_t i = 0; i < count; i++)
                    ptr[i] = c2;
                return ptr;
            }
        };

        static constexpr inline bool eq(char_type a, char_type b) {
            return a == b;
        };
        static constexpr inline bool lt(char_type a, char_type b) {
            return a < b;
        };

        static char_type* move(char_type* dest, const char_type* src, std::size_t count) noexcept {
            return reinterpret_cast<char_type*>(memmove(dest, src, count * sizeof(char_type)));
        };
        static char_type* copy(char_type* dest, const char_type* src, std::size_t count) noexcept {
            return reinterpret_cast<char_type*>(memcpy(dest, src, count * sizeof(char_type)));
        };

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t count) noexcept {
            if constexpr (sizeof(char_type) == 1)
                return memcmp(s1, s2, count);
            else {
                for (; count; count--, s1++, s2++)
                    if (*s1 != *s2)
                        return lt(*s1, *s2) ? -1 : 1;
            }
        };

        static constexpr std::size_t length(const char_type* s) noexcept {
            std::size_t res = 0;
            for (; !eq(*s, char_type()); res++, s++);
            return res;
        };

        static constexpr const char_type* find(const char_type* ptr, std::size_t count, const char_type& ch) noexcept {
            const char_type* res = ptr;
            for (; !eq(*res, ch); res++);
            return res;
        };

        static constexpr char_type to_char_type(int_type c) noexcept {
            return static_cast<char_type>(c);
        };
        static constexpr int_type to_int_type(char_type c) noexcept{
            return static_cast<int_type>(c);
        };

        static constexpr inline bool eq_int_type(int_type c1, int_type c2) noexcept{
            return c1 == c2;
        };

        static constexpr inline int_type eof() noexcept {
            return static_cast<int_type>(EOF);
        };
        static constexpr int_type not_eof(int_type e) noexcept {
            return !eq_int_type(e, eof()) ? e : !eof();
        };
    };

    template <typename CharT, typename IntT, typename StreamPosT>
    struct char16_traits : public char8or32_traits<CharT, IntT, StreamPosT> {
    private:
        using Primary_char_traits = char8or32_traits<CharT, IntT, StreamPosT>;

    public:
        static void assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        };

        static char_type* assign(char_type* ptr, std::size_t count, char_type c2) noexcept {
            if constexpr (sizeof(char_type) == 2)
                return reinterpret_cast<char_type*>(wmemset(reinterpret<wchar_t*>(ptr), c2, count));
            else return Primary_char_traits::assign(ptr, count, c2);
        };

        static constexpr inline bool eq(char_type a, char_type b) {
            return a == b;
        };
        static constexpr inline bool lt(char_type a, char_type b) {
            return a < b;
        };

        using Primary_char_traits::copy;
        using Primary_char_traits::move;

        static constexpr int compare(const char_type* s1, const char_type* s2, std::size_t count) noexcept {
            if constexpr (is_same_v<char_type, wchar_t>)
                return wmemcmp(s1, s2, count);
            else {
                for (; count; count--, s1++, s2++)
                    if (*s1 != *s2)
                        return lt(*s1, *s2) ? -1 : 1;
            }
        };

        static constexpr std::size_t length(const char_type* s) noexcept {
            if constexpr (is_same_v<char_type, wchar_t>)
                return wcslen(s);
            else {
                std::size_t res = 0;
                for (; !eq(*s, char_type()); res++, s++);
                return res;
            }
        };

        static constexpr const char_type* find(const char_type* ptr, std::size_t count, const char_type& ch) noexcept {
            if constexpr (is_same_v<char_type, wchar_t>)
                return std::wmemchr(ptr, ch, count);
            else {
                const char_type* res = ptr;
                for (; !eq(*res, ch); res++);
                return res;
            }
        };

        static constexpr char_type to_char_type(int_type c) noexcept {
            return c;
        };
        static constexpr int_type to_int_type(char_type c) noexcept {
            return c;
        };

        static constexpr inline bool eq_int_type(int_type c1, int_type c2) noexcept {
            return c1 == c2;
        };

        static constexpr inline int_type eof() noexcept {
            return WEOF;
        };

        static constexpr int_type not_eof(int_type e) noexcept {
            return !eq_int_type(e, eof()) ? e : !eof();
        };
    };

    template <typename CharT>
    struct char_traits           : public char8or32_traits<CharT,    int,                 std::streampos>    {};
    template <> 
    struct char_traits<wchar_t>  : public char16_traits   <wchar_t,  std::wint_t,         std::wstreampos>   {};
    template <> 
    struct char_traits<char16_t> : public char16_traits   <char16_t, std::uint_least16_t, std::u16streampos> {};
    template <> 
    struct char_traits<char32_t> : public char8or32_traits<char32_t, std::uint_least32_t, std::u32streampos> {};

#endif // !OCT_CHAR_TRAITS

    template <typename Str>
    struct short_string_val {
        using value_type = typename Str::value_type;
        using size_type  = typename Str::size_type;
        using pointer    = typename Str::pointer;

        static constexpr size_type buf_capacity = (sizeof(pointer) + sizeof(size_type) - sizeof(value_type)) / sizeof(value_type);
        mutable value_type buf[buf_capacity + 1];
    };

    template <typename Str>
    struct long_string_val {
        using size_type = typename Str::size_type;
        using pointer   = typename Str::pointer;

        pointer begin;
        size_type val_capacity;
    };

    template <typename Str>
    struct string_val{
    private:
        using Short_mode = short_string_val<Str>;
        using Long_mode  = long_string_val<Str>;

    public:
        using value_type = typename Str::value_type;
        using size_type  = typename Str::size_type;
        using pointer    = typename Str::pointer;

        mutable union {
            Short_mode short_mode;
            Long_mode  long_mode;
        } mode;
        size_type val_size = 0;
        static constexpr size_type buf_capacity = Short_mode::buf_capacity;

        inline bool long_mode_engaged() noexcept {
            return val_size > Short_mode::buf_capacity;
        }

        string_val() = default;

        string_val(size_type count) : mode{}, val_size(count) {
            if (long_mode_engaged())
                mode.long_mode.val_capacity = count * 2;
        }

        inline pointer& get_ptr() noexcept {
           if (long_mode_engaged()) return mode.long_mode.begin;
           else return reinterpret_cast<pointer&>(std::ref(mode.short_mode.buf));
        }

        inline size_type get_capacity() noexcept {
            return long_mode_engaged() ? mode.long_mode.val_capacity : Short_mode::buf_capacity;
        }
        
        inline pointer to_short() {
            pointer temp = mode.long_mode.begin;
            return Str::traits_type::move(mode.short_mode.buf, temp, val_size);
        }
    };

#define OCT_STRING_VAL_PACK_UNWRAP pointer&   val          = val_pack.second.get_ptr();                                                                        \
                                   size_type& val_size     = val_pack.second.val_size;                                                                         \
                                   size_type  buf_capacity = string_val<basic_string>::buf_capacity;                                                           \
                                   size_type& val_capacity = val_pack.second.long_mode_engaged() ? val_pack.second.mode.long_mode.val_capacity : buf_capacity; \
                                   Alty&      alloc        = val_pack.get_first();

    template <typename CharT, 
              typename Traits    = char_traits<CharT>, 
              typename Allocator = trivial_allocator<CharT>
    >
    class basic_string {
    private:
        using Alty = rebind_alloc_t<Allocator, CharT>;        //模板参数集成
        using Alty_traits = std::allocator_traits<Alty>;             //空间配置器萃取器

    public:
        static_assert(is_character_or_byte_or_bool_v<CharT>, "basic_string requires character or byte or bool");

        using traits_type     = Traits;
        using value_type      = CharT;
        using allocator_type  = Allocator;
        using size_type       = typename Alty_traits::size_type;;
        using difference_type = typename Alty_traits::difference_type;;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = typename Alty_traits::pointer;
        using const_pointer   = typename Alty_traits::const_pointer;
        using iterator        = continuous_memory_iterator<basic_string>;

        static constexpr size_type npos = -1;

    private:
        friend class iterator;

        mutable compressed_pair<Alty, string_val<basic_string>> val_pack;

    public:
        basic_string() : val_pack(Alty()) {};

        explicit basic_string(const allocator_type& al) 
            : val_pack(static_cast<Alty>(al)) {}

        basic_string(size_type count, value_type ch, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), count) {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                val_capacity = val_size * 2;
                val = Alty_traits::allocate(alloc, val_capacity);
            }
            traits_type::assign(val, count, ch);
        }

        basic_string(const_pointer begin, const_pointer end, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), std:::distance(end - begin)) {
            if (!begin || !end)
                Pointer_error();
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                val_capacity = val_size * 2;
                val = Alty_traits::allocate(alloc, val_capacity);
            }
            traits_type::copy(val, begin, val_size);
        }

        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        basic_string(ForwardIt begin, ForwardIt end, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), std::distance(begin, end)) {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                val_capacity = val_size * 2;
                val = Alty_traits::allocate(alloc, val_capacity);
            }
            for (difference_type i = 0; begin!-= end; begin++)
                val[i] = *begin;
        }

        basic_string(const_pointer begin, size_type count, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), count) {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                val_capacity = val_size * 2;
                val = Alty_traits::allocate(alloc, val_capacity);
            }
            traits_type::copy(val, begin, val_size);
        }

        basic_string(const_pointer begin, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), traits_type::length(begin)) {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                 val_capacity = val_size * 2;
                 val = Alty_traits::allocate(alloc, val_capacity);
            }
            traits_type::copy(val, begin, val_size);
        }

        explicit basic_string(const basic_string& another, const allocator_type& al = allocator_type())
            : basic_string(another.val_pack.second.get_ptr(), another.val_pack.second.val_size, al) {}

        explicit basic_string(basic_string&& another, const allocator_type& al = allocator_type())
            : val_pack(static_cast<Alty>(al), another.val_pack.second.val_size) noexcept {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged()) {
                val_pack.second.mode = another.val_pack.second.mode;
                another.val_pack.second.long_mode.begin = nullptr;
                another.val_pack.second.val_size = 0;
            }
            else {
                traits_type::copy(val, another.val_pack.second.get_ptr(), val_size);
            }
        }

        basic_string(const basic_string& another, size_type pos, const allocator_type& al = allocator_type())
            : basic_string(another.val_pack.second.get_ptr(), pos + 1, al) {}

        basic_string(std::initializer_list<value_type> ini_list, const allocator_type& al = allocator_type())
            : basic_string(ini_list.begin(), ini_list.end(), al) {}

        ~basic_string() {
            if (val_pack.second.long_mode_engaged()) {
                OCT_STRING_VAL_PACK_UNWRAP
                Alty_traits::deallocate(alloc, val, val_pack.second.mode.long_mode.val_capacity);
            }
        }

        basic_string& operator=(const basic_string& another) {
            if (this == &another)
                return *this;
            this->~basic_string();
            OCT_STRING_VAL_PACK_UNWRAP
            val_size = another.val_pack.second.val_size;
            if (val_pack.second.long_mode_engaged()) 
                val_capacity = another.val_pack.second.mode.long_mode.val_capacity;
            traits_type::copy(val, another.val_pack.second.get_ptr(), val_size);
            return *this;
        }

        basic_string& operator=(basic_string&& another) noexcept {
            if (this == &another)
                return *this;
            this->~basic_string();
            OCT_STRING_VAL_PACK_UNWRAP
            val_size = another.val_pack.second.val_size;
            if (val_pack.second.long_mode_engaged()) {
                val_pack.second.mode = another.val_pack.second.mode;
                another.val_pack.second.mode.long_mode.begin = nullptr;
                another.val_pack.second.val_size = 0;
            }
            else {
                traits_type::copy(val, another.val_pack.second.get_ptr(), val_size);
            }
            return *this;
        }

        basic_string& assign(size_type count, CharT ch) {
            return *this = basic_string(count, ch);
        }

        basic_string& assign(const_pointer begin, const_pointer end) {
            return *this = basic_string(begin, end);
        }

        template<typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        basic_string& assign(ForwardIt begin, ForwardIt end) {
            return *this = basic_string(begin, end);
        }

        basic_string& assign(const_pointer begin, size_type count) {
            return *this = basic_string(begin, count);
        }

        basic_string& assign(const_pointer begin) {
            return *this = basic_string(begin);
        }

        basic_string& assign(const basic_string& another) {
            return *this = another;
        }

        basic_string& assign(basic_string&& another) noexcept {
            return *this = std::move(another);
        }

        basic_string& assign(const basic_string& another, size_type pos) {
            return *this = basic_string(another.val_pack.second.get_ptr(), pos + 1)
        }

        basic_string& assign(std::initializer_list<value_type> ini_list) {
            return *this = basic_string(ini_list.begin(), ini_list.end()) {}
        }

        allocator_type get_allocator() const noexcept {
            return allocator_type();
        }

        reference at(size_type pos) const {//安全的下标访问
            if (pos >= val_size)
                Subscript_error();
            return val_pack.second.get_ptr()[pos];
        }

        reference operator[](size_type pos) const {
            return val_pack.second.get_ptr()[pos];
        }

        reference front() const {//访问头元素
            OCT_STRING_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return val_pack.second.get_ptr()[0];
        }

        reference back() const {//访问尾元素
            OCT_STRING_VAL_PACK_UNWRAP
            if (!val_size)
                Logic_error();
            return val_pack.second.get_ptr()[val_size - 1];
        }

        pointer data() const noexcept {//返回底层指针
            return val_pack.second.get_ptr();
        }

        const_pointer c_str() const noexcept {
            OCT_STRING_VAL_PACK_UNWRAP
            if (val_pack.second.long_mode_engaged() && val_capacity == val_size) {
                reserve(2 * val_size);
            }
            val[val_size] = char_type();
            return val;
        }

        iterator begin() const noexcept {
            return iterator(val_pack.second.get_ptr());
        }

        iterator end() const noexcept {
            return iterator(val_pack.second.get_ptr() + val_pack.second.val_size);
        }

        bool empty() const noexcept {//返回是否为空
            return !val_pack.second.val_size;
        }

        size_type size() const noexcept {//返回实际长度
            return val_pack.second.val_size;
        }

        size_type length() const noexcept {//返回实际长度
            return val_pack.second.val_size;
        }

        void reserve(size_type capacity) {
            bool long_mode = val_pack.second.long_mode_engaged();
            if (long_mode && capacity <= val_capacity || !long_mode)
                return;
            OCT_STRING_VAL_PACK_UNWRAP
            if constexpr (is_trivial_allocator_v<Alty>)
                val = alloc.reallocate(val, capacity);
            else {
                pointer temp = Alty_traits::allocate(alloc, capacity);//分配新地址
                traits_type::copy(temp, val, val_size);
                Alty_traits::deallocate(alloc, val, val_capacity);//解分配老地址
                val = temp;
            }
            val_capacity = capacity;
        }

        size_type capacity() const noexcept {
            return val_pack.second.long_mode_engaged() ? val_pack.second.long_mode.val_capacity
                : val_pack.second.short_mode::buf_capacity;
        }

        size_type shrink_to_fit() const noexcept {
            if (val_pack.second.long_mode_engaged())
                resize(val_pack.second.long_mode.val_capacity());
        }

        basic_string substr(size_type pos = 0, size_type count = npos) const {
            return basic_string(val_pack.second.get_ptr() + pos, val_pack.second.val_size - pos < count ? val_pack.second.val_size : count);
        }

        basic_string operator[](std::initializer_list<value_type> ini_list) const {
            size_type pos = *ini_list.begin(), count = *(ini_list.begin() + 1) - pos;
            return substr(pos, count);
        }

        void clear() {
            OCT_STRING_VAL_PACK_UNWRAP
            traits_type::assign(val_pack.second.get_ptr(), val_size, char_type());
            val_size = 0;
        }
        /*
        basic_string& insert(size_type index, size_type count, value_type ch) {}
        basic_string& insert(size_type index, const_pointer s) {}
        basic_string& insert(size_type index, const_pointer s, size_type count) {}
        basic_string& insert(size_type index, const basic_string& str) {}
        basic_string& insert(size_type index, const basic_string& str, size_type s_index, size_type count) {}
        basic_string& insert(size_type index, const basic_string& str, size_type s_index, size_type count = npos) {}
        iterator insert(iterator pos, value_type ch) {}
        iterator insert(iterator pos, value_type ch) {}
        iterator insert(iterator pos, size_type count, value_type ch) {}
        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        iterator insert(iterator pos, ForwardIt first, ForwardIt last) {}
        iterator insert(iterator pos, std::initializer_list<value_type> ini_list) {}
        basic_string& erase(size_type index = 0, size_type count = npos) {}
        iterator erase(iterator pos) {}
        iterator erase(iterator first, iterator last) {}
        void push_back(value_type ch) {}
        void pop_back() {}
        basic_string& append(size_type count, value_type ch) {}
        basic_string& append(const_pointer s, size_type count) {}
        basic_string& append(const_pointer s) {}
        basic_string& append(const basic_string& str) {}
        basic_string& append(const basic_string& str, size_type pos, size_type count = npos) {}
        template <typename ForwardIt, std::enable_if_t<is_forward_iterator_v<ForwardIt>, int> = 0>
        basic_string& append(InputIt first, InputIt last) {}
        basic_string& append(std::initializer_list<value_type> ini_list) {}
        basic_string& operator+=(const basic_string& str) {}
        basic_string& operator+=(value_type ch) {}
        basic_string& operator+=(const value_type* s) {}
        basic_string& operator+=(std::initializer_list<value_type> ini_list) {}
        size_type copy(pointer dest, size_type count, size_type pos = 0) const {}
        void resize(size_type count, value_type ch) {}
        void swap(basic_string& other) noexcept {}
        template <typename As, typename Container>
        Container split_to(char ch) {}
        void trim() {}
        */
    private:
        [[noreturn]] static void Pointer_error() {
            std::out_of_range msg("invalid pointer");
        }

        [[noreturn]] static void Logic_error() {
            std::logic_error msg("invalid string size");
        }

        [[noreturn]] static void Subscript_error() {
            std::out_of_range msg("invalid string subscript");
        }

        [[noreturn]] static void Iterator_error() {
            std::out_of_range msg("invalid string iterator");
        }
    };

    using string    = basic_string<char,     std::char_traits<char>,     trivial_allocator<char>>;
    using wstring   = basic_string<wchar_t,  std::char_traits<wchar_t>,  trivial_allocator<wchar_t>>;
    using u16string = basic_string<char16_t, std::char_traits<char16_t>, trivial_allocator<char16_t>>;
    using u32string = basic_string<char32_t, std::char_traits<char32_t>, trivial_allocator<char32_t>>;
}

#endif // !OCT_STRING