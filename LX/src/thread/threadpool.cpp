#include "threadpool.h"

thread_pool::thread_pool(const std::size_t preallocated_thread_count) :
    m_accept_jobs(true),
    m_allocated_thread_count(preallocated_thread_count),
    m_busy_threads(0)
{
    rescale_thread_pool();
}

thread_pool::~thread_pool()
{
    destroy();
}

void thread_pool::rescale_thread_pool()
{
    m_thread_pool.reserve(m_allocated_thread_count);

    while (m_thread_pool.size() < m_allocated_thread_count)
    {
        m_thread_pool.emplace_back(&thread_pool::run, this);
    }
}

void thread_pool::destroy()
{
    {
        std::unique_lock lock(m_lock);
        m_accept_jobs = false;
    }
    m_data_condition.notify_all();

    for (auto& thread : m_thread_pool)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    m_thread_pool.clear();
}

void thread_pool::push(const std::function<void()>& func, const std::source_location& location)
{
    if (func)
    {
        {
            std::unique_lock lock(m_lock);
            m_job_stack.push({func, location});

            if (m_allocated_thread_count - m_busy_threads < m_job_stack.size())
            {
                if (m_accept_jobs && m_allocated_thread_count < max_pool_size)
                {
                    ++m_allocated_thread_count;
                    rescale_thread_pool();
                }
            }
        }
        m_data_condition.notify_one();
    }
}

void thread_pool::run()
{
    for (;;)
    {
        std::function<void()> job;
        {
            std::unique_lock lock(m_lock);
            m_data_condition.wait(lock, [this]()
            {
                return !m_job_stack.empty() || !m_accept_jobs;
            });

            if (!m_accept_jobs && m_job_stack.empty())
                break;

            if (!m_job_stack.empty())
            {
                auto [m_func, m_source_location] = m_job_stack.top();
                m_job_stack.pop();
                job = std::move(m_func);
                ++m_busy_threads;
            }
        }

        if (job)
        {
            job();
            {
                std::unique_lock lock(m_lock);
                --m_busy_threads;
            }
        }
    }
}