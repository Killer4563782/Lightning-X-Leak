#include "hooking/hooking.hpp"
#include "memory/pointers.h"

void hooks::searchlight_crash(void* a1, CPed* ped)
{
	if (!ped || !g_pointers->g_get_searchlight(ped))
		return;

	return g_hooking->get_original<hooks::searchlight_crash>()(a1, ped);
}