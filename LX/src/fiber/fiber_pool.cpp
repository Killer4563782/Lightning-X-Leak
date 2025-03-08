#include "fiber_pool.hpp"
#include "script.hpp"
#include "script_mgr.hpp"

fiber_pool::fiber_pool(const size_t num_fibers) 
{
	m_num_fibers = num_fibers; 
	for (size_t i = 0; i < num_fibers; ++i)
	{
		g_script_mgr.add_script(std::make_unique<script>(&fiber_func));
	}
}

void fiber_pool::queue_job(std::function<void()> func)
{
	if (func)
	{
		std::lock_guard lock(m_mutex);
		m_jobs.push(std::move(func));
	}
}

void fiber_pool::fiber_tick()
{
	std::unique_lock lock(m_mutex);
	if (!m_jobs.empty())
	{
		std::remove_reference_t<std::function<void()>&> job = std::move(m_jobs.top());
		m_jobs.pop();
		lock.unlock();

		std::invoke(std::move(job));
	}
}

void fiber_pool::fiber_func()
{
	while (true)
	{
		g_fiber_pool->fiber_tick();
		script::get_current()->yield();
	}
}

size_t fiber_pool::get_total_fibers() const
{
	return m_num_fibers;
}

size_t fiber_pool::get_used_fibers() const
{
	return m_jobs.size();
}

void fiber_pool::reset()
{
	std::lock_guard lock(m_mutex);

	while (!m_jobs.empty())
		m_jobs.pop();
}
