#pragma once

namespace utl
{
    template<class _Ty>
    using scope_ptr = std::unique_ptr<_Ty>;

    template<class _Ty, class... _Args>
    constexpr utl::scope_ptr<_Ty> make_scope(_Args&& ...args)
    {
        return std::make_unique<_Ty>(std::forward<_Args>(args)...);
    }

    template<class _Ty>
    using ref_ptr = std::shared_ptr<_Ty>;

    template<class _Ty, class... _Args>
    constexpr utl::ref_ptr<_Ty> make_ref(_Args&& ...args)
    {
        return std::make_shared<_Ty>(std::forward<_Args>(args)...);
    }

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
        static utl::scope_ptr<_Ty> _instance;

    public:
        static utl::scope_ptr<_Ty> &inst()
        {
            if (!_instance)
                _instance.reset(new _Ty());
            return _instance;
        }
    };

    //https://stackoverflow.com/questions/6942273/how-to-get-a-random-element-from-a-c-container
    template<typename _Iter, typename _RandomGenerator>
	_Iter random_pick(_Iter start, _Iter end, _RandomGenerator& g) 
	{
		std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
		std::advance(start, dis(g));
		return start;
	}

	template<typename _Iter>
	_Iter random_pick(_Iter start, _Iter end)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		return random_pick(start, end, gen);
	}
}