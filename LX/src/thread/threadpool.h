#pragma once
#include "pch/pch.h"
#include <source_location>
// if this limit is hit you did something wrong coding wise.
constexpr auto max_pool_size = 32u;

struct thread_pool_job
{
	std::function<void()> m_func;
	std::source_location m_source_location;
};

class thread_pool
{
	std::atomic<bool> m_accept_jobs;
	std::condition_variable m_data_condition;

	std::stack<thread_pool_job> m_job_stack;
	std::mutex m_lock;
	std::vector<std::thread> m_thread_pool;

	// the amount of threads active in the pool
	std::atomic<size_t> m_allocated_thread_count;
	// the amount of threads currently on a job
	std::atomic<size_t> m_busy_threads;

public:
	explicit thread_pool(std::size_t preallocated_thread_count = 4);
	~thread_pool();

	void destroy();
	void push(const std::function<void()>& func, const std::source_location& location = std::source_location::current());

	std::pair<size_t, size_t> usage() const
	{
		return {m_busy_threads, m_allocated_thread_count};
	}

private:
	void run();
	void rescale_thread_pool();
};

inline std::unique_ptr<thread_pool> g_thread_pool{};