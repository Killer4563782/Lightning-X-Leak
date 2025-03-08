#include "commands/manager.hpp"
#include "hooking/hooking.hpp"

void* hooks::task_fall_constructor(uint64_t _this, int flags)
{
	if ("no_ragdoll"_LC->m_toggle)
	{
		flags = DisableNMFall | HasAirControl | DisableLandRoll | DisableParachuteTask;
	}

	if (const auto cmd = "graceful_landing"_LVC; cmd->m_toggle)
	{
		if (cmd->m_index == 0) // Roll
		{
			flags |= DisableNMFall | HasAirControl | DampZVelocity;
			flags &= ~(BeastFall | DisableLandRoll);
		}
		else if (cmd->m_index == 1) // Beast
		{
			flags |= DisableNMFall | HasAirControl | BeastFall | DampZVelocity;
		}
	}

	return g_hooking->get_original<task_fall_constructor>()(_this, flags);
}
