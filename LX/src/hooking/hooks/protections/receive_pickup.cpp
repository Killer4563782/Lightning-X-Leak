#include "hooking/hooking.hpp"
#include "services/reactions/reactions.h"

bool hooks::receive_pickup(rage::netObject* netobject, void* unk, CPed* ped)
{
	if (pickup.process(nullptr, "Pickup"))
		return false;

	return g_hooking->get_original<receive_pickup>()(netobject, unk, ped);
}
