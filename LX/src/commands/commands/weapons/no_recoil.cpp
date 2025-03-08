#include "commands/types/looped_command.hpp"

namespace commands
{
	class no_recoil final : looped_command
	{
		using looped_command::looped_command;

		CWeaponInfo* p_modified_weapon = nullptr;
		uint32_t og_recoil_hash        = 0;
		uint32_t og_recoil_hash_fp     = 0;

		void on_tick() override
		{
			if (!self::local_ped)
				return;

			if (auto* const weapon_mgr = self::local_ped->m_weapon_manager)
			{
				if (p_modified_weapon != weapon_mgr->m_weapon_info)
				{
					if (p_modified_weapon)
					{
						p_modified_weapon->m_recoil_shake_hash              = og_recoil_hash;
						p_modified_weapon->m_recoil_shake_hash_first_person = og_recoil_hash_fp;
					}

					p_modified_weapon = weapon_mgr->m_weapon_info;

					if (weapon_mgr->m_weapon_info)
					{
						og_recoil_hash    = weapon_mgr->m_weapon_info->m_recoil_shake_hash;
						og_recoil_hash_fp = weapon_mgr->m_weapon_info->m_recoil_shake_hash_first_person;
						weapon_mgr->m_weapon_info->m_recoil_shake_hash              = 0;
						weapon_mgr->m_weapon_info->m_recoil_shake_hash_first_person = 0;
					}
				}
			}
		}

		void on_disable() override
		{
			if (self::local_ped && p_modified_weapon)
			{
				p_modified_weapon->m_recoil_shake_hash              = og_recoil_hash;
				p_modified_weapon->m_recoil_shake_hash_first_person = og_recoil_hash_fp;
				p_modified_weapon                                   = nullptr;
			}
		}
	};

	no_recoil g_no_recoil("No Recoil", "no_recoil", {});
}
