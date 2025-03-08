#pragma once

namespace scr_locals
{
	class script_local
	{
	public:
		explicit script_local(rage::scrThread* thread, std::size_t index) :
			m_index(index),
			m_stack(thread->m_stack)
		{}

		explicit script_local(PVOID stack, std::size_t index) :
			m_index(index),
			m_stack(stack)
		{}

		explicit script_local(std::size_t index) :
			m_index(index),
			m_stack(nullptr)
		{}

		script_local set(rage::scrThread* thread) const
		{
			return script_local(thread, m_index);
		}

		script_local set(void* stack) const
		{
			return script_local(stack, m_index);
		}

		script_local at(std::ptrdiff_t index) const
		{
			return script_local(m_stack, m_index + index);
		}

		script_local at(std::ptrdiff_t index, std::size_t size) const
		{
			return script_local(m_stack, m_index + 1 + (index * size));
		}

		template<typename T>
		std::enable_if_t<std::is_pointer_v<T>, T> as()
		{
			return static_cast<T>(get());
		}

		template<typename T>
		std::enable_if_t<std::is_lvalue_reference_v<T>, T> as()
		{
			return *static_cast<std::add_pointer_t<std::remove_reference_t<T>>>(get());
		}

	
		void* get()
		{
			return reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(m_stack) + (m_index * sizeof(uintptr_t)));
		}
		std::size_t m_index;
		PVOID m_stack;
	};
}
