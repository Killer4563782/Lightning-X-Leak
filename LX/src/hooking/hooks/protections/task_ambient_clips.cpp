#include "hooking/hooking.hpp"

int hooks::task_ambient_clips(uint64_t _this, int a2, int a3)
{
	// enter crash func
	// TODO: this doesn't block the crash completely

	if (*reinterpret_cast<uint64_t*>(_this + 0x100))
	{
		return g_hooking->get_original<task_ambient_clips>()(_this, a2, a3);
	}

	return 0;
}
