#include "hooking/hooking.hpp"

bool hooks::fragment_physics_crash_2(__m128* object0, __m128* object1)
{
	if (!object0 || !object1)
	{
		return false;
	}

	return g_hooking->get_original<fragment_physics_crash_2>()(object0, object1);
}
