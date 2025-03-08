#include "security.h"
#include "memory/pointers.h"
#include "core/util.hpp"
#include "memory/signature.hpp"

namespace security
{
	void bsod()
	{
		typedef NTSTATUS(NTAPI* RtlAdjustPrivilege)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
		typedef NTSTATUS(NTAPI* NtRaiseHardError)(NTSTATUS, ULONG, ULONG OPTIONAL, PULONG_PTR, ULONG, PULONG);

		auto adjust_privilege = reinterpret_cast<RtlAdjustPrivilege>(GetProcAddress(LoadLibraryA("ntdll.dll"), "RtlAdjustPrivilege"));
		auto raise_hard_error = reinterpret_cast<NtRaiseHardError>(GetProcAddress(GetModuleHandle("ntdll.dll"), "NtRaiseHardError"));

		BOOLEAN enabled;
		ULONG response;
		adjust_privilege(19, TRUE, FALSE, &enabled);
		raise_hard_error(STATUS_ILLEGAL_INSTRUCTION, 0, 0, nullptr, 6, &response);
	}

	void debugger_detected(DetectReason code)
	{
		tripped_dbg = true;
		LOG_DEBUG("Debugger Detected: {}", static_cast<int>(code));

#ifndef _DEBUG
		bsod();
#endif

		TerminateProcess(GetCurrentProcess(), 0);
	}

	bool check_for_debugger()
	{
#ifndef _DEBUG
		if (tripped_dbg)
			return true;

		if (BOOL debugger_present = FALSE; IsDebuggerPresent() || (CheckRemoteDebuggerPresent(GetCurrentProcess(), &debugger_present) && debugger_present))
		{
			debugger_detected(DetectReason::DebuggerPresent);
			return true;
		}

		SetLastError(0);
		OutputDebugStringA(" ");
		if (GetLastError() != 0)
		{
			debugger_detected(DetectReason::OutputDbgString);
			return true;
		}

		__try
		{
			DebugBreak();
			debugger_detected(DetectReason::DbgBreak);
			return true;
		}
		__except (GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) {}
#endif 

		return false;
	}


	
	void init()
	{
		DisableThreadLibraryCalls(g_module);
	}

	void uninit()
	{

	}
};