#include "hooking/hooking.hpp"
#include "memory/pointers.h"

void* hooks::render_ped(__int64 renderer, CPed* ped, __int64 a3, __int64 a4)
{
	if (*(int*)(((__int64)(*g_pointers->g_draw_handler_mgr) + 0x14730)) >= 499)
	{
		return nullptr;
	}
	else
	{
		return g_hooking->get_original<hooks::render_ped>()(renderer, ped, a3, a4);
	}
}