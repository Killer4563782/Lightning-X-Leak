#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

#include "rage/scrValue.hpp"
#include "rage/vector.hpp"

namespace rage
{
	class scrNativeCallContext
	{
	public:
		constexpr void reset()
		{
			m_arg_count = 0;
			m_data_count = 0;
		}

		template <typename T>
		constexpr void push_arg(T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_args) + (m_arg_count++)) = std::forward<T>(value);
		}

		template <typename T>
		constexpr T& get_arg(std::size_t index)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			return *reinterpret_cast<T*>(reinterpret_cast<std::uint64_t*>(m_args) + index);
		}

		template <typename T>
		constexpr void set_arg(std::size_t index, T&& value)
		{
			static_assert(sizeof(T) <= sizeof(std::uint64_t));
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(reinterpret_cast<std::uint64_t*>(m_args) + index) = std::forward<T>(value);
		}

		template <typename T>
		constexpr T* get_return_value()
		{
			return reinterpret_cast<T*>(m_return_value);
		}

		template <typename T>
		constexpr void set_return_value(T&& value)
		{
			*reinterpret_cast<std::remove_cv_t<std::remove_reference_t<T>>*>(m_return_value) = std::forward<T>(value);
		}

		constexpr void copy_params_out()
		{
			while (m_data_count--)
			{
				m_origin[m_data_count][0].Float = m_buffer[m_data_count].x;
				m_origin[m_data_count][1].Float = m_buffer[m_data_count].y;
				m_origin[m_data_count][2].Float = m_buffer[m_data_count].z;
			}
		}

		//Return result, if applicable
		scrValue* m_return_value;
		//Parameter count
		std::uint32_t m_arg_count;
		//Pointer to parameter values
		scrValue* m_args;
		std::int32_t m_data_count;
		scrValue* m_origin[4];
		fvector4 m_buffer[4];
	};
	static_assert(sizeof(scrNativeCallContext) == 0x80);

	using scrNativeHash = std::uint64_t;
	using scrNativePair = std::pair<scrNativeHash, scrNativeHash>;
	using scrNativeHandler = void(*)(scrNativeCallContext*);
}