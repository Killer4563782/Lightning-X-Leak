#pragma once

class script
{
	std::string m_name;
	bool m_enabled;
	bool m_toggleable;
	bool m_done;

public:
	using func_t = std::function<void()>;

	explicit script(const func_t& func, const std::string& name, bool toggleable = true, std::optional<size_t> stack_size = std::nullopt);
	explicit script(func_t func, std::optional<size_t> stack_size = std::nullopt);
	~script();

	const char* name() const;
	bool is_enabled() const;
	void set_enabled(bool toggle);
	bool* toggle_ptr();

	bool is_toggleable() const;

	bool is_done() const;

	void tick();
	void yield(std::optional<high_resolution_clock::duration> time = std::nullopt);
	static script* get_current();

private:
	void fiber_func();

	void* m_script_fiber;
	void* m_main_fiber;
	func_t m_func;
	std::optional<high_resolution_clock::time_point> m_wake_time;
};
