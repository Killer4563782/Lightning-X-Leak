#include "hooking/hooking.hpp"

void hooks::invalid_ped_component_draw(rage::grmShaderGroup* a1, int a2, int64_t a3)
{
	if (!a1->var_array)
	{
		return;
	}

	return g_hooking->get_original<invalid_ped_component_draw>()(a1, a2, a3);
}