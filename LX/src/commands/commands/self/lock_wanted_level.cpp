#include "commands/types/number/looped_int_command.hpp"
#include "util/patches.hpp"

namespace commands
{
	class lock_wanted_level final : looped_int_command
	{
		using looped_int_command::looped_int_command;
		
		void on_tick() override
		{
			if (!self::local_ped)
				return;

			if (m_value > 5)
			{
				self::local_ped->m_player_info->m_wanted_level_display = m_value;
				self::local_ped->m_player_info->m_wanted_level = 5;
			}
			else
			{
				self::local_ped->m_player_info->m_wanted_level_display = m_value;
				self::local_ped->m_player_info->m_wanted_level = m_value;
			}
		}

		void on_disable() override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_player_info->m_wanted_level_display = self::local_ped->m_player_info->m_wanted_level;
		}
	};

	lock_wanted_level g_lock_wanted_level("Lock Wanted Level", "lock_wanted_level", {}, false, false, 0, 5);
}
