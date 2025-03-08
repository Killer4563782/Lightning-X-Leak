#include <hooking/hooking.hpp>

void ModifyPoolSize(rage::atArray<CPoolSize>& sizes, const char* poolName, unsigned int size)
{
	for (auto& entry : sizes)
	{
		if (!_stricmp(entry.m_pool, poolName))
		{
			entry.m_size = size;
			return;
		}
	}

	sizes.append({ (char*)poolName, size });
}

rage::fwConfigManagerImpl<CGameConfig>* hooks::ReadGameConfig(rage::fwConfigManagerImpl<CGameConfig>* manager, const char* file)
{
	return g_hooking->get_original<ReadGameConfig>()(manager,file);
}