#include <hooking/hooking.hpp>

void* hooks::CreatePool(void* pool, int size, const char* name, int unk1, int unk2, bool unk3)
{
	g_hooking->get_original<CreatePool>()(pool,size,name,unk1,unk2,unk3);
	g_Pools.insert({ pool, g_LastPoolHash });
	return pool;
}