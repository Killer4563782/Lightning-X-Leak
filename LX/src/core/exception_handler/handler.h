#pragma once

namespace exceptions
{
#ifdef _DEBUG
	inline uint32_t g_old_error_mode;
#endif
	inline void* g_handler;

	extern LONG exception_handler(EXCEPTION_POINTERS* exception_info);
	extern void init_exception_handler();
	extern void uninit_exception_handler();
}
