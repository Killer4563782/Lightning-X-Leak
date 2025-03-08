#include "hooking/hooking.hpp"

int hooks::task_parachute_object(uint64_t _this, int a2, int a3)
{
	if (a2 == 1 && a3 == 1) // enter crash func
	{
		if (const auto ptr = *reinterpret_cast<uint64_t*>(_this + 16))
		{
			if (const auto ptr2 = *reinterpret_cast<uint64_t*>(ptr + 80))
			{
				if (auto ptr3 = *reinterpret_cast<uint64_t*>(ptr2 + 64))
				{
					return g_hooking->get_original<task_parachute_object>()(_this, a2, a3);
				}
			}
		}
		return 0;
	}
	return g_hooking->get_original<task_parachute_object>()(_this, a2, a3);
}
