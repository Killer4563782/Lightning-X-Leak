#include "customcar.h"
#include <memory/pointers.h>

namespace rage
{
	CrossState* CrossState::ptr()
	{
		return (CrossState*)(((uintptr_t)(*g_pointers->g_network)->m_game_session_ptr) + 0x5584);
	}

	CrossState::CrossState() : version_major(current_version_major), version_minor(current_version_minor)
	{

	}

	CrossState& CrossState::get()
	{
		if (ptr() == nullptr)
		{
			
			*ptr() = *new CrossState();
		}
		return *ptr();
	}

	CustomVehicleModel*& CrossState::getCustomVehicleModelsTail()
	{
		auto pNode = &custom_vehicle_models_head;
		while (*pNode && (*pNode)->next)
		{
			pNode = &(*pNode)->next;
		}
		return *pNode;
	}
}