#include "handler.h"

#include "stack_trace.h"
#include "minhook/hde/hde64.h"
#include "util/util.h"

namespace exceptions
{
	LONG exception_handler(EXCEPTION_POINTERS* exception_info)
	{
		auto exception_code = exception_info->ExceptionRecord->ExceptionCode;

		if (exception_code == EXCEPTION_BREAKPOINT || exception_code == DBG_PRINTEXCEPTION_C || exception_code == DBG_PRINTEXCEPTION_WIDE_C)
		{
#ifdef _DEBUG
			return EXCEPTION_CONTINUE_SEARCH;
#else
			return EXCEPTION_CONTINUE_EXECUTION;
#endif
		}

		std::string st = stack_trace::create_stack_trace(exception_info);
		LOG(error, st);

		if (IsBadReadPtr(reinterpret_cast<void*>(exception_info->ContextRecord->Rip), 8))
		{
			auto return_address_ptr = reinterpret_cast<uint64_t*>(exception_info->ContextRecord->Rsp);
			if (IsBadReadPtr(return_address_ptr, 8))
			{
				LOG(error, "Failed to recover from exception. The game is probably going to crash.");
				return EXCEPTION_CONTINUE_SEARCH;
			}

			exception_info->ContextRecord->Rip = *return_address_ptr;
			exception_info->ContextRecord->Rsp += 8;
		}
		else
		{
			hde64s opcode;
			hde64_disasm(reinterpret_cast<void*>(exception_info->ContextRecord->Rip), &opcode);
			if (opcode.flags & F_ERROR)
			{
				LOG(error, "Failed to recover from exception. The game is probably going to crash.");
				return EXCEPTION_CONTINUE_SEARCH;
			}

			if (opcode.opcode == 0xFF && opcode.modrm_reg == 4) 
			{
				auto return_address_ptr = (uint64_t*)exception_info->ContextRecord->Rsp;
				if (IsBadReadPtr(reinterpret_cast<void*>(return_address_ptr), 8))
				{
					LOG(error, "Cannot resume execution, crashing");
					return EXCEPTION_CONTINUE_SEARCH;
				}
				else
				{
					exception_info->ContextRecord->Rip = *return_address_ptr;
					exception_info->ContextRecord->Rsp += 8;
				}
			}
			else
			{
				exception_info->ContextRecord->Rip += opcode.len;
			}
		}

		return EXCEPTION_CONTINUE_EXECUTION;
	}

	void init_exception_handler()
	{
		SymInitialize(GetCurrentProcess(), nullptr, true);

#ifdef _DEBUG
		g_old_error_mode = SetErrorMode(0);
#endif
		g_handler = SetUnhandledExceptionFilter(&exception_handler);
	}

	void uninit_exception_handler()
	{
		SymCleanup(GetCurrentProcess());

#ifdef _DEBUG
		SetErrorMode(g_old_error_mode);
#endif
		SetUnhandledExceptionFilter(reinterpret_cast<decltype(&exception_handler)>(g_handler));
	}
}
