#include "hooking/hooking.hpp"

void hooks::invalid_decal(uintptr_t c_ped, int component)
{
	if (c_ped && component == 2)
	{
		if (const auto ptr = *reinterpret_cast<uintptr_t*>(c_ped + 0x48); ptr)
		{
			if (const auto ptr2 = *reinterpret_cast<uintptr_t*>(ptr + 0x30); ptr2)
			{
				if (*reinterpret_cast<uintptr_t*>(ptr2 + 0x2C8) == 0)
				{
					return;
				}
			}
		}
	}

	return g_hooking->get_original<invalid_decal>()(c_ped, component);
}