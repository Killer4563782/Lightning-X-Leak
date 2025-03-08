#pragma once

class vmt_hook
{
public:
	explicit vmt_hook(void* obj, size_t num_funcs);
	~vmt_hook();

	void hook(size_t index, void* func);
	void unhook(size_t index);

	template<typename T>
	T get_original(size_t index);

	void enable() const;
	void disable() const;

private:
	void*** m_object;
	size_t m_num_funcs;

	void** m_original_table;
	std::unique_ptr<void*[]> m_new_table;
};

template<typename T>
T vmt_hook::get_original(size_t index)
{
	return static_cast<T>(m_original_table[index]);
}
