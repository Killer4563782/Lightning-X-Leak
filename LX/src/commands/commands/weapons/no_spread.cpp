#include "commands/types/looped_command.hpp"

namespace commands
{
	class no_spread final : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* p_modified_weapon = nullptr;
		float og_spread_value          = 0.0f;

		void on_tick() override
		{
			if (!self::local_ped)
				return;

			if (auto* const weapon_mgr = self::local_ped->m_weapon_manager)
			{
				if (p_modified_weapon != weapon_mgr->m_weapon_info)
				{
					if (p_modified_weapon)
						p_modified_weapon->m_accuracy_spread = og_spread_value;

					p_modified_weapon = weapon_mgr->m_weapon_info;

					if (weapon_mgr->m_weapon_info)
					{
						og_spread_value                              = weapon_mgr->m_weapon_info->m_accuracy_spread;
						weapon_mgr->m_weapon_info->m_accuracy_spread = 0.0f;
					}
				}
			}
		}

		void on_disable() override
		{
			if (self::local_ped && p_modified_weapon)
			{
				p_modified_weapon->m_accuracy_spread = og_spread_value;
				p_modified_weapon                    = nullptr;
			}
		}
	};

	no_spread g_no_spread("No Spread", "no_spread", {});
}
