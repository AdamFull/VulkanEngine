#pragma once

namespace utl
{
    /**
     * @brief Makes child non copyable
     * 
     */
    class non_copyable
    {
    protected:
        non_copyable() = default;
        virtual ~non_copyable() = default;

    public:
        non_copyable(const non_copyable &) = delete;
        non_copyable &operator=(const non_copyable &) = delete;
    };

    /**
     * @brief Makes child non movable
     * 
     */
    class non_movable
    {
    protected:
        non_movable() = default;
        virtual ~non_movable() = default;

    public:
        non_movable(non_movable &&) noexcept = delete;
        non_movable &operator=(non_movable &&) noexcept = delete;
    };

    /**
     * @brief Maskes child non copyable and non movable
     * 
     */
    class non_copy_movable : public non_copyable, public non_movable
    {
    protected:
        non_copy_movable() = default;
        virtual ~non_copy_movable() = default;
    };

    /**
     * @brief Makes singleton from child
     * 
     * @tparam _Ty Children class
     */
    template <class _Ty>
    class singleton : public non_copy_movable
    {
    protected:
        static scope_ptr<_Ty> _instance;

    public:
        static scope_ptr<_Ty> &inst()
        {
            if (!_instance)
                _instance.reset(new _Ty());
            return _instance;
        }
    };
}