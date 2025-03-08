#include "commands/math.h"
#include "commands/types/looped_command.hpp"

namespace commands
{
	class inf_clip final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (const auto c_ped_inventory = self::local_ped->m_inventory; c_ped_inventory) {
				c_ped_inventory->m_infinite_clip = true;
			}
		}

		void on_disable() override
		{
			if (const auto c_ped_inventory = self::local_ped->m_inventory; c_ped_inventory) {
				c_ped_inventory->m_infinite_clip = false;
			}
		}
	};

	inf_clip g_inf_clip("Infinite Clip", "inf_clip", {});
}
