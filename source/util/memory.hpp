#pragma once

namespace utl
{
    template<class _Ty>
    class allocator : public std::allocator<_Ty>
    {
    public:
        using value_type = _Ty;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

        constexpr allocator() noexcept {}
        constexpr allocator(const allocator&) noexcept = default;
        template <class _Other>
        constexpr allocator(const allocator<_Other>&) noexcept {}
        ~allocator() = default;

        allocator& operator=(const allocator&) = default;

        _Ty* allocate(size_t _Count)
        {
            return std::allocator<_Ty>::allocate(_Count);
        }

        void deallocate(_Ty* const _Ptr, const size_t _Count) 
        {
            //std::stringstream ss{};
            //ss << "Deallocating: entity " << typeid(_Ty).name() << ", size: " << _Count << ", address: 0x" << static_cast<const void*>(_Ptr);
            //logger::log(ELogLevel::eInfo, ss.str());
            return std::allocator<_Ty>::deallocate(_Ptr, _Count);
        }
    };

    template<class _Ty>
    using scope_ptr = std::unique_ptr<_Ty>;

    template<class _Ty, class... _Args>
    constexpr utl::scope_ptr<_Ty> make_scope(_Args&& ...args)
    {
        return std::make_unique<_Ty>(std::forward<_Args>(args)...);
    }

    template<class _Ty>
    using ref_ptr = std::shared_ptr<_Ty>;

    template<class _Ty>
    using weak_ptr = std::weak_ptr<_Ty>;

    template<class _Ty, class... _Args>
    constexpr utl::ref_ptr<_Ty> make_ref(_Args&& ...args)
    {
        return std::allocate_shared<_Ty>(utl::allocator<_Ty>(), std::forward<_Args>(args)...);
    }
}