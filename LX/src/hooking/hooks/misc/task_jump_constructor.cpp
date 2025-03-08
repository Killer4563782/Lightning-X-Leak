#include "commands/manager.hpp"
#include "hooking/hooking.hpp"

int64_t hooks::task_jump_constructor(uint64_t _this, int flags)
{
	if (const auto cmd = "super_jump"_LVC; cmd->m_toggle)
	{
		if (cmd->m_index == 0) // Flag
		{
			flags |= SuperJump;
		}
		else if (cmd->m_index == 1) // Beast
		{
			flags |= SuperJump | BeastJump;
		}
	}

	return g_hooking->get_original<task_jump_constructor>()(_this, flags);
}
