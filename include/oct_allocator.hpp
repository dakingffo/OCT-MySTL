#pragma once

#ifndef OCT_ALLOCATOR
#define OCT_ALLOCATOR

#include <type_traits>
#include <cstddef> 

namespace oct {
    using std::byte;

    template <typename Ty = byte>
    struct allocator {
    public:
        static_assert(std::is_object_v<Ty>, "allocator requires object types");

        using value_type      = Ty;
        using pointer         = Ty*;
        using const_pointer   = const Ty*;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

    public:
        allocator() = default;

        template <typename Elem>
        allocator(const allocator<Elem>& another) {}

        pointer allocate(size_type n) {
            pointer result = static_cast<pointer>(::operator new(n * sizeof(value_type)));
            if (result == nullptr)
                Allocate_error();
            else return result;
        }

        void deallocate(pointer ptr, size_type n) {
            ::operator delete(ptr, n);
        }

    protected:
        [[noreturn]] static void Allocate_error() {
            std::bad_alloc();
        }
    };

    template <typename Ty = byte>
    struct trivial_allocator : public allocator<Ty> {
    public:
        static_assert(std::is_trivially_copyable_v<Ty>, "trivial_allocator requires trivially copyable object types");

        pointer reallocate(pointer old, size_type n) {
            pointer result = static_cast<pointer>(realloc(old, n * sizeof(value_type)));
            if (result == nullptr)
                Allocate_error();
            else return result;
        }

        pointer memory_move(pointer old, pointer dest, size_type n) {
            pointer result = static_cast<pointer>(memmove(dest, old, n * sizeof(value_type)));
            if (result == nullptr)
                Allocate_error();
            else return result;
        }

        pointer memory_copy(const_pointer source, pointer dest, size_type n) {
            pointer result = static_cast<pointer>(memcpy(dest, source, n * sizeof(value_type)));
            if (result == nullptr)
                Allocate_error();
            else return result;
        }

        pointer memory_set(pointer dest, value_type target, size_type n) {
            static_assert(is_character_or_byte_or_bool_v<value_type>, "trivial_allocator memory_set requires character or byte or bool types");
            pointer result = static_cast<pointer>(memset(dest, target, n * sizeof(value_type)));
            if (result == nullptr)
                Allocate_error();
            else return result;
        }
    };

    /*
    template <typename Ty = byte>
    struct multilevel_allocator {
    public:
        static_assert(std::is_object_v<Ty>, "multilevel_allocator requires object types");

        using value_type      = Ty;
        using pointer         = Ty*;
        using const_pointer   = const Ty*;
        using reference       = Ty&;
        using const_reference = const Ty&;
        using size_type       = std::size_t;
        using difference_type = std::ptrdiff_t;

        struct tidy_list_node {
            tidy_list_node* next;

            tidy_list_node(tidy_list_node* n) : next(n) {}

            ~tidy_list_node() {
                if (next) {
                    next->~tidy_list_node();
                    free(next);
                }
            }
        };

        struct mallocator {
        public:
            using value_type = typename multilevel_allocator<Ty>::value_type;
            using pointer = typename multilevel_allocator<Ty>::pointer;
            using const_pointer = typename multilevel_allocator<Ty>::const_pointer;
            using reference = typename multilevel_allocator<Ty>::reference;
            using const_reference = typename multilevel_allocator<Ty>::const_reference;
            using size_type = typename multilevel_allocator<Ty>::size_type;
            using difference_type = typename multilevel_allocator<Ty>::difference_type;

        public:
            static pointer allocate(size_type bytes) {
                pointer result = static_cast<pointer>(::operator new(bytes));
                if (result == nullptr)
                    Allocate_error();
                else return result;
            }

            static void deallocate(pointer ptr, size_type bytes) {
                ::operator delete(ptr, bytes);
            }

        protected:
            [[noreturn]] static void Allocate_error() {
                std::bad_alloc();
            }
        };

        struct memory_pool {
        public:
            using value_type = typename multilevel_allocator<Ty>::value_type;
            using pointer = typename multilevel_allocator<Ty>::pointer;
            using const_pointer = typename multilevel_allocator<Ty>::const_pointer;
            using reference = typename multilevel_allocator<Ty>::reference;
            using const_reference = typename multilevel_allocator<Ty>::const_reference;
            using size_type = typename multilevel_allocator<Ty>::size_type;
            using difference_type = typename multilevel_allocator<Ty>::difference_type;

            enum ARGS {
                ALIGN = 8, MAX_BYTES = 128, NFREELIST = 16
            };

            union Page {
                Page* next;
                byte memory[1];
            };

            friend struct multilevel_allocator;

        private:
            static Page* free_list[ARGS::NFREELIST];
            static byte* list_begin;
            static byte* list_end;
            static size_type heap_size;

        public:
            static pointer allocate(size_type bytes) {
                std::cout << "A";
                Page** free_list_ptr = free_list + Free_list_index(bytes);
                Page* result = *free_list_ptr;
                if (result) {
                    *free_list_ptr = result->next;
                    return reinterpret_cast<pointer>(result);
                }
                else return Refill(Round_up(bytes));
            }

            static void deallocate(pointer ptr, size_type bytes) {
                if (!ptr)
                    return;
                Page** free_list_ptr = free_list + Free_list_index(bytes);
                Page* page = reinterpret_cast<Page*>(ptr);
                page->next = *free_list_ptr;
                *free_list_ptr = page;
            }

        private:
            inline static size_type Round_up(size_type bytes) {
                return (bytes + ARGS::ALIGN - 1) & ~(ARGS::ALIGN - 1);
            }

            inline static size_type Free_list_index(size_type bytes) {
                return (bytes + ARGS::ALIGN - 1) / ARGS::ALIGN - 1;
            }

            static pointer Refill(size_type bytes) {
                size_type pages = 20;
                byte* chunk = Chunk_alloc(bytes, pages);
                if (pages == 1)
                    return reinterpret_cast<pointer>(chunk);
                else {
                    Page** free_list_ptr = free_list + Free_list_index(bytes);
                    Page* current = nullptr, * next = nullptr;
                    pointer result = reinterpret_cast<pointer>(chunk);
                    *free_list_ptr = next = reinterpret_cast<Page*>(chunk + bytes);
                    for (size_type i = 1; i != pages - 1; i++) {
                        current = next;
                        next = reinterpret_cast<Page*>(reinterpret_cast<char*>(next) + bytes);
                        current->next = next;
                    }
                    next->next = nullptr;
                    return result;
                }
            }

            static byte* Chunk_alloc(size_type bytes, size_type& pages) {
                byte* result;
                size_type total_bytes = bytes * pages;
                size_type left_bytes = list_end - list_begin;
                if (left_bytes >= total_bytes) {
                    result = list_begin;
                    list_begin += total_bytes;
                    return result;
                }
                else if (left_bytes >= bytes) {
                    pages = static_cast<size_type>(left_bytes / bytes);
                    total_bytes = bytes * pages;
                    result = list_begin;
                    list_begin += total_bytes;
                    return result;
                }
                else {
                    size_t bytes_to_get = 2 * total_bytes + Round_up(heap_size >> 4);
                    if (left_bytes > 0) {
                        Page** free_list_ptr = free_list + Free_list_index(left_bytes);
                        reinterpret_cast<Page*>(list_begin)->next = *free_list_ptr;
                        *free_list_ptr = reinterpret_cast<Page*>(list_begin);
                    }
                    byte* new_memory = reinterpret_cast<byte*>(malloc(sizeof(tidy_list_node) + bytes_to_get));
                    if (!new_memory) {
                        Page** free_list_ptr = nullptr;
                        Page* page = nullptr;
                        for (size_type i = bytes; i <= MAX_BYTES; i += ALIGN) {
                            free_list_ptr = free_list + Free_list_index(i);
                            page = *free_list_ptr;
                            if (page) {
                                *free_list_ptr = page->next;
                                list_begin = reinterpret_cast<byte*>(page);
                                list_end = list_begin + i;
                                return Chunk_alloc(bytes, pages);
                            }
                        }
                        list_end = nullptr;
                        new_memory = reinterpret_cast<byte*>(multilevel_allocator<Ty>::allocator<1>::allocate(sizeof(tidy_list_node) + bytes_to_get));
                    }
                    tidy_list_node* tidy_list_node_ptr = reinterpret_cast<tidy_list_node*>(new_memory);
                    new (tidy_list_node_ptr) tidy_list_node(nullptr);
                    (tidy_list_back ? tidy_list_back->next : tidy_list_dummy.next) = tidy_list_node_ptr;
                    tidy_list_back = tidy_list_node_ptr;
                    list_begin = new_memory + sizeof(tidy_list_node);
                    heap_size += bytes_to_get;
                    list_end = list_begin + bytes_to_get;
                    return Chunk_alloc(bytes, pages);
                }
            };
        };

        template <int level>
        struct allocator {};

        template <>
        struct allocator<1> : public mallocator {};
        template <>
        struct allocator<2> : public memory_pool {};

    private:
        static size_type count;
        static tidy_list_node tidy_list_dummy;
        static tidy_list_node* tidy_list_back;

    public:
        multilevel_allocator() {
            count++;
        }

        template <typename Elem>
        multilevel_allocator(const multilevel_allocator<Elem>& another) {
            count++;
        }

        ~multilevel_allocator() {
            if (!(--count)) {
                tidy_list_dummy.~tidy_list_node();
                tidy_list_dummy.next = nullptr;
                tidy_list_back = nullptr;
            }
        }

        pointer allocate(size_type n) {
            size_type bytes = n * sizeof(value_type);
            if (bytes > 128) return allocator<1>::allocate(bytes);
            else return allocator<2>::allocate(bytes);
        }

        void deallocate(pointer ptr, size_type n) {
            size_type bytes = n * sizeof(value_type);
            if (bytes > 128) allocator<1>::deallocate(ptr, bytes);
            else allocator<2>::deallocate(ptr, bytes);
        }
    };
    
    template <typename Ty>
    typename multilevel_allocator<Ty>::size_type multilevel_allocator<Ty>::count = 0;

    template <typename Ty>
    typename multilevel_allocator<Ty>::tidy_list_node multilevel_allocator<Ty>::tidy_list_dummy = typename multilevel_allocator<Ty>::tidy_list_node(nullptr);

    template <typename Ty>
    typename multilevel_allocator<Ty>::tidy_list_node* multilevel_allocator<Ty>::tidy_list_back = nullptr;

    template <typename Ty>
    typename multilevel_allocator<Ty>::memory_pool::Page* 
        multilevel_allocator<Ty>::memory_pool::free_list[multilevel_allocator<Ty>::memory_pool::ARGS::NFREELIST] = {};

    template <typename Ty>
    byte* multilevel_allocator<Ty>::memory_pool::list_begin = nullptr;

    template <typename Ty>
    byte* multilevel_allocator<Ty>::memory_pool::list_end = nullptr;

    template <typename Ty>
    typename multilevel_allocator<Ty>::size_type multilevel_allocator<Ty>::memory_pool::heap_size = 0;
    */
};

#endif // ! OCT_ALLOCATOR