#pragma once

class mem {
public:
	template <typename PtrT = void*>
	mem* set_ptr(PtrT p)
	{
		m_ptr = reinterpret_cast<void*>(p);
		return this;
	}

	mem(void* p = nullptr)
	{
		set_ptr(p);
	}

	mem(const uintptr_t p)
	{
		set_ptr(p);
	}

	template <typename T>
	std::enable_if_t<std::is_pointer_v<T>, T> as()
	{
		return static_cast<T>(m_ptr);
	}

	template <typename T>
	std::enable_if_t<std::is_lvalue_reference_v<T>, T> as()
	{
		return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(m_ptr);
	}

	template <typename T>
	std::enable_if_t<std::is_same_v<T, uintptr_t>, T> as()
	{
		return T(m_ptr);
	}

	mem add(size_t offset)
	{
		return {as<uintptr_t>() + offset};
	}

	mem sub(size_t offset)
	{
		return {as<uintptr_t>() - offset};
	}

	mem rip()
	{
		return add(as<int32_t&>()).add(4);
	}

	mem mov()
	{
		return add(3).rip();
	}

	mem cmp()
	{
		return add(3).rip();
	}

	mem lea()
	{
		return add(2).rip();
	}

	mem call()
	{
		return add(1).rip();
	}

	operator bool() noexcept
	{
		return m_ptr;
	}

	uint64_t read_instruction(uint64_t address, int opcode_size, int opcode_length)
	{
		return *reinterpret_cast<int*>(address + opcode_size) + address + opcode_length;
	}

	void* m_ptr{};
};