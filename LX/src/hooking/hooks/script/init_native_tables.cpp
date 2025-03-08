#include "hooking/hooking.hpp"
#include "hooking/nativeHooking/native_hooks.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

bool hooks::init_native_tables(rage::scrProgram* program)
{
	const bool ret = g_hooking->get_original<init_native_tables>()(program);

	// ensure that we aren't hooking SHV threads
	if (program->m_code_size && program->m_code_blocks)
	{
		g_script_patcher_service->on_script_load(program);
		g_native_hooks->hook_program(program);
	}

	return ret;
}
