#include "hooking/hooking.hpp"
#include "fiber/script_mgr.hpp"

bool hooks::run_script_threads(uint32_t ops_to_execute)
{
	g_script_mgr.tick();
	return g_hooking->get_original<run_script_threads>()(ops_to_execute);
}