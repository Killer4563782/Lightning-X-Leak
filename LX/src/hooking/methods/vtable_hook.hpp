#pragma once

class vtable_hook
{
public:
	explicit vtable_hook(void** vft, size_t num_funcs);
	~vtable_hook();


	void hook(size_t index, void* func);
	void unhook(size_t index);

	template<typename T>
	T get_original(size_t index);

	inline void** get_original_table()
	{
		return m_backup_table.get();
	}

	void enable();
	void disable();

private:
	size_t m_num_funcs;
	void** m_table;
	std::unique_ptr<void*[]> m_backup_table;
	std::unique_ptr<void*[]> m_hook_table;
};

template<typename T>
inline T vtable_hook::get_original(size_t index)
{
	return static_cast<T>(m_backup_table[index]);
}
