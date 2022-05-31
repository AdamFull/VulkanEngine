#pragma once
#include "function.hpp"
#include "helpers.hpp"

namespace Engine
{
	/**
	 * @brief Future class for returning data from thread
	 * 
	 * @tparam _Ty Data type
	 */
    template <typename _Ty>
    class CFuture
    {
    public:
        CFuture() noexcept = default;

        CFuture(std::future<_Ty> &&_future) noexcept : __future(std::move(_future))
        {
        }

        bool has_value() const noexcept
        {
            return __future.valid() || __current;
        }

        _Ty &get() noexcept
        {
            if (__future.valid())
                __current = __future.get();
            return *__current;
        }

        constexpr explicit operator bool() const noexcept { return has_value(); }
        constexpr operator _Ty &() const noexcept { return *get(); }

        _Ty &operator*() noexcept { return get(); }
        _Ty &operator->() noexcept { return get(); }

        bool operator==(const CFuture &rhs) const noexcept
        {
            return __future == rhs.__future && __current == rhs.__current;
        }

        bool operator!=(const CFuture &rhs) const noexcept
        {
            return !operator==(rhs);
        }

    private:
        std::future<_Ty> __future;
        std::optional<_Ty> __current;
    };

	/**
	 * @brief Worker class of thread pool
	 * 
	 */
    class CWorker
	{
	private:
		std::atomic<bool> _destroying{false};
		std::atomic<bool> _waiting{true};
		std::thread _thread;
		std::queue<utl::function<void()>> _work_queue;
		std::mutex _queue_mutex;
		std::condition_variable _condition;

		/**
		 * @brief Worker main loop
		 * 
		 */
		void loop();

	public:
		CWorker();
		~CWorker();

		/**
		 * @brief Pushes lambda function without parameters
		 * 
		 * @tparam _Lambda Lambda function, or class type
		 * @param function Worker function
		 */
		template<class _Lambda>
		void push(_Lambda&& function)
		{
			std::lock_guard<std::mutex> lock(_queue_mutex);
			_work_queue.push(std::move(function));
			_condition.notify_one();
		}

		/**
		 * @brief Pushes lambda function with parameters
		 * 
		 * @tparam _Lambda Lambda function, or class type
		 * @tparam _Types Lambda function variadic arguments
		 * @param function Worker function
		 * @param args Variadic arguments
		 */
		template<class _Lambda, class... _Types>
		void push(_Lambda&& function, _Types&& ...args)
		{
			push([function, ...args = std::forward<_Types>(args)] { function(std::forward<_Types>(args)...); });
		}

		/**
		 * @brief Wait until all work items have been finished
		 * 
		 */
		void wait()
		{
			std::unique_lock<std::mutex> lock(_queue_mutex);
			_condition.wait(lock, [this]() { return _work_queue.empty(); });
		}

		bool is_free()
		{
			return _waiting;
		}
	};

	/**
	 * @brief Thread pool implementation class
	 * 
	 */
    class CThreadPool : public utl::singleton<CThreadPool>
    {
    public:
        CThreadPool();
		CThreadPool(const size_t& count);
		~CThreadPool();

		/**
		 * @brief Pushes functional object as worker task
		 * 
		 * @tparam _Types Variadic arguments
		 * @param args Arguments
		 */
        template<class... _Types>
        void push(_Types&& ...args)
        {
            _workers.at(_current)->push(std::forward<_Types>(args)...);
            if(_current < _total)
                _current = 0;
        }

		/**
		 * @brief Pushes functional object with return type for assynchronous result processing 
		 * 
		 * @param work Functional worker
		 * @param args Variadic arguments
		 * @return CFuture<_Kty> Future object for attempting access to result 
		 */
		template<class _Ty, class... _Types, typename _Kty = std::invoke_result_t<std::decay_t<_Ty>, std::decay_t<_Types>...>, typename = std::enable_if_t<!std::is_void_v<_Ty>>>
		CFuture<_Kty> submit(_Ty&& work, _Types&& ...args)
		{
			auto task_promise = make_ref<std::promise<_Kty>>();
			auto _future = task_promise->get_future();

			_workers.at(_current)->push([work = std::forward<_Ty>(work), args..., task_promise]()
			{
				try { task_promise->set_value(work(args...)); }
				catch(...)
				{
					try { task_promise->set_exception(std::current_exception()); }
                    catch (...) { }
				}
			});

			if(_current < _total)
                _current = 0;

			return CFuture<_Kty>(std::move(_future));
		}

		/**
		 * @brief Set the worker count
		 * 
		 * @param count Thread number
		 */
        void set_worker_count(const size_t& count);

		/**
		 * @brief Wait while work will be done
		 * 
		 */
        void wait();
    private:
        std::vector<scope_ptr<CWorker>> _workers;
        size_t _total{0}, _current{0};
    };
}