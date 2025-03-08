#pragma once

class fiber_pool
{
public:
	explicit fiber_pool(size_t num_fibers);
	~fiber_pool() = default;

	void queue_job(std::function<void()> func);

	void fiber_tick();
	static void fiber_func();

	size_t get_total_fibers() const;
	size_t get_used_fibers() const;

	void reset();

private:
	std::recursive_mutex m_mutex;
	std::stack<std::function<void()>> m_jobs;
	size_t m_num_fibers;
};

inline std::unique_ptr<fiber_pool> g_fiber_pool{};
