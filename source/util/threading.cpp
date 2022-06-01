#include "threading.hpp"

using namespace engine;

template <>
scope_ptr<CThreadPool> utl::singleton<CThreadPool>::_instance{nullptr};

CWorker::CWorker()
{
    _thread = std::thread(&CWorker::loop, this);
}

CWorker::~CWorker()
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

void CWorker::loop()
{
    while (true)
    {
        utl::function<void()> work;
        {
            std::unique_lock<std::mutex> lock_begin(_queue_mutex);
            _condition.wait(lock_begin, [this]
                            { return !_work_queue.empty() || _destroying; });
            if (_destroying)
                break;
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

CThreadPool::CThreadPool()
{
    set_worker_count(std::thread::hardware_concurrency());
}

CThreadPool::CThreadPool(const size_t &count)
{
    set_worker_count(count);
}

CThreadPool::~CThreadPool()
{
    wait();
}

void CThreadPool::set_worker_count(const size_t &count)
{
    _total = count;
    _workers.clear();
    for (auto i = 0; i < _total; i++)
        _workers.emplace_back(make_scope<CWorker>());
}

void CThreadPool::wait()
{
    for (auto &worker : _workers)
        worker->wait();
}