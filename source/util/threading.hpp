#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include "function.hpp"
#include <future>
#include <queue>

namespace utl
{
    template <typename _Ty>
    class future
    {
    public:
        future() noexcept = default;

        future(std::future<_Ty> &&_future) noexcept : __future(std::move(_future))
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

        bool operator==(const future &rhs) const noexcept
        {
            return __future == rhs.__future && __current == rhs.__current;
        }

        bool operator!=(const future &rhs) const noexcept
        {
            return !operator==(rhs);
        }

    private:
        std::future<_Ty> __future;
        std::optional<_Ty> __current;
    };

    class worker
	{
	private:
		std::atomic<bool> _destroying{false};
		std::atomic<bool> _waiting{true};
		std::thread _thread;
		std::queue<function<void()>> _work_queue;
		std::mutex _queue_mutex;
		std::condition_variable _condition;

		void loop()
		{
			while (true)
			{
				function<void()> work;
				{
					std::unique_lock<std::mutex> lock_begin(_queue_mutex);
					_condition.wait(lock_begin, [this] { return !_work_queue.empty() || _destroying; });
					if (_destroying) break;
					work = _work_queue.front();
					_waiting = false;
				}

				work();

				{
					std::lock_guard<std::mutex> lock_end(_queue_mutex);
					_work_queue.pop();
					_waiting = true;
					_condition.notify_one();
				}
			}
		}

	public:
		worker()
		{
			_thread = std::thread(&worker::loop, this);
		}

		~worker()
		{
			if (_thread.joinable())
			{
				wait();
				_queue_mutex.lock();
				_destroying = true;
				_condition.notify_one();
				_queue_mutex.unlock();
				_thread.join();
			}
		}

		template<class _Lambda>
		void push(_Lambda&& function)
		{
			std::lock_guard<std::mutex> lock(_queue_mutex);
			_work_queue.push(std::move(function));
			_condition.notify_one();
		}

		template<class _Lambda, class... _Types>
		void push(_Lambda&& function, _Types&& ...args)
		{
			push([function, ...args = std::forward<_Types>(args)] { function(std::forward<_Types>(args)...); });
		}

		// Wait until all work items have been finished
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

    class threadpool
    {
    public:
        threadpool() { set_worker_count(std::thread::hardware_concurrency()); }
		threadpool(const size_t& count) { set_worker_count(count); }
		~threadpool() { wait(); }


        template<class... _Types>
        void push(_Types&& ...args)
        {
            _workers.at(_current)->push(std::forward<_Types>(args)...);
            if(_current < _total)
                _current = 0;
        }

		template<class _Ty, class... _Types, typename _Kty = std::invoke_result_t<std::decay_t<_Ty>, std::decay_t<_Types>...>, typename = std::enable_if_t<!std::is_void_v<_Ty>>>
		future<_Kty> submit(_Ty&& work, _Types&& ...args)
		{
			auto task_promise = std::make_shared<std::promise<_Kty>>();
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

			return future<_Kty>(std::move(_future));
		}

        void set_worker_count(const size_t& count)
        {
            _total = count;
            _workers.clear();
			for (auto i = 0; i < _total; i++)
				_workers.emplace_back(std::make_unique<worker>());
        }

        void wait()
        {
            for (auto &worker : _workers)
				worker->wait();
        }
    private:
        std::vector<std::unique_ptr<worker>> _workers;
        size_t _total{0}, _current{0};
    };
}