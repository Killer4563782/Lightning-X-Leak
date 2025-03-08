#include "hooking/hooking.hpp"

void* hooks::GetPoolItem(void* pool)
{
	auto item = g_hooking->get_original<GetPoolItem>()(pool);

	if (item)
		return item;

	if (g_Pools.count(pool))
	{
		auto hash = g_Pools[pool];
	}

	return nullptr;
}