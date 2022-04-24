#pragma once
#include <memory>
#include <map>
#include <iostream>

namespace utl
{
    template <typename _Ty>
    struct track_alloc : std::allocator<_Ty>
    {
        using pointer = _Ty*;
        using const_pointer = const pointer;
        using size_type = std::allocator<_Ty>::size_type;

        template <typename U>
        struct rebind
        {
            typedef track_alloc<U> other;
        };

        track_alloc() {}

        template <typename U>
        track_alloc(track_alloc<U> const &u) : std::allocator<_Ty>(u) {}

        pointer allocate(size_type size, const_pointer = 0)
        {
            void *p = std::malloc(size * sizeof(_Ty));
            if (p == 0)
                throw std::bad_alloc();
            return static_cast<pointer>(p);
        }

        void deallocate(pointer p, size_type)
        {
            std::free(p);
        }
    };

    using track_type = std::map<void *, std::size_t, std::less<void *>, track_alloc<std::pair<void *const, std::size_t>>>;

    struct track_printer
    {
        track_type *track;
        track_printer(track_type *track) : track(track) {}
        ~track_printer();
    };
}