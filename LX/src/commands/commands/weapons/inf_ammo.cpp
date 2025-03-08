#include "commands/types/looped_command.hpp"

namespace commands
{
	class inf_ammo final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (const auto c_ped_inventory = self::local_ped->m_inventory; c_ped_inventory) {
				c_ped_inventory->m_infinite_ammo = true;
			}
		}

		void on_disable() override
		{
			if (const auto c_ped_inventory = self::local_ped->m_inventory; c_ped_inventory) {
				c_ped_inventory->m_infinite_ammo = false;
			}
		}
	};

	inf_ammo g_inf_ammo("Infinite Ammo", "inf_ammo", {});
}
