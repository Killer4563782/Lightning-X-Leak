#pragma once
#include "..\conversion_table.h"
#include "script/scrNativeHandler.hpp"

class custom_call_context : public rage::scrNativeCallContext {
public:
	constexpr custom_call_context() : scrNativeCallContext(&m_return, 0, m_arguments)
	{
#ifndef _DEBUG
		memset(m_arguments, NULL, sizeof(m_arguments));
#endif
		m_return = {};
		m_args = m_arguments;
	}

private:
	rage::scrValue m_return;
	rage::scrValue m_arguments[32];
};

class native_invoker
{
	static inline rage::scrNativeHandler m_handlers[g_crossmap.size()];
	static inline bool m_are_handlers_cached{false};

public:
	constexpr native_invoker()
	{
	}

	constexpr void begin_call()
	{
		m_call_context.reset();
	}

	template <int Index, bool FixVectors>
	constexpr void end_call()
	{
		// TODO: try to get rid of this
		if (!m_are_handlers_cached)
			cache_handlers();

		m_handlers[Index](&m_call_context);
		if constexpr (FixVectors)
			this->fix_vectors();
	}

	void lua_end_call(int index, bool FixVector) //REDO!
	{
		if (!m_are_handlers_cached)
			cache_handlers();

		m_handlers[index](&m_call_context);
		if (FixVector)
		{
			this->fix_vectors();
		}
	}

	template <typename T>
	constexpr void push_arg(T&& value)
	{
		m_call_context.push_arg(std::forward<T>(value));
	}

	template <typename T>
	constexpr T& get_return_value()
	{
		return *m_call_context.get_return_value<T>();
	}

	void fix_vectors();

	static void __declspec(noinline) cache_handlers();

	static rage::scrNativeHandler* get_handlers()
	{
		if (!m_are_handlers_cached)
			cache_handlers();

		return m_handlers;
	}

	template <int Index, bool FixVectors, typename Ret, typename... Args>
	static constexpr FORCEINLINE Ret invoke(Args&&... args)
	{
		native_invoker invoker{};

		invoker.begin_call();
		(invoker.push_arg(std::forward<Args>(args)), ...);
		invoker.end_call<Index, FixVectors>();

		if constexpr (!std::is_same_v<Ret, void>)
		{
			return invoker.get_return_value<Ret>();
		}
	}

	custom_call_context m_call_context{};
};