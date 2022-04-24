#pragma once
#include <memory>

namespace utl
{
    class non_copyable
    {
    protected:
        non_copyable() = default;
        virtual ~non_copyable() = default;

    public:
        non_copyable(const non_copyable &) = delete;
        non_copyable &operator=(const non_copyable &) = delete;
    };

    class non_movable
    {
    protected:
        non_movable() = default;
        virtual ~non_movable() = default;

    public:
        non_movable(non_movable &&) noexcept = delete;
        non_movable &operator=(non_movable &&) noexcept = delete;
    };

    class non_copy_movable : public non_copyable, public non_movable
    {
    protected:
        non_copy_movable() = default;
        virtual ~non_copy_movable() = default;
    };

    template <class _Ty>
    class singleton : public non_copy_movable
    {
    protected:
        static std::unique_ptr<_Ty> _instance;

    public:
        static std::unique_ptr<_Ty> &getInstance()
        {
            if (!_instance)
                _instance.reset(new _Ty());
            return _instance;
        }
    };
}