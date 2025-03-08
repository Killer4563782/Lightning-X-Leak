#pragma once

class detour_hook
{
public:
	explicit detour_hook();
	explicit detour_hook(const std::string& name, void* detour);
	explicit detour_hook(const std::string& name, void* target, void* detour);
	~detour_hook() noexcept;

	void set_instance(const std::string& name, void* detour);
	void set_instance(const std::string& name, void* target, void* detour);

	void set_target_and_create_hook(void* target);

	void enable();
	void disable() const;

	template<typename T>
	T get_original()
	{
		return static_cast<T>(m_original);
	}

	void fix_hook_address();

private:
	void create_hook();

	std::string m_name;
	void* m_original;
	void* m_target;
	void* m_detour;
};
