#include "commands/types/command.hpp"

namespace commands
{
	class max_health final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_health = self::local_ped->m_maxhealth;
		}
	};

	class max_armor final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_armor = self::local_ped->m_player_info->m_max_armor;
		}
	};
	
	max_health g_max_health("Max Health", "max_health", "Max your health", std::nullopt, true);
	max_armor g_max_armor("Max Armor", "max_armor", "Max your armor", std::nullopt, true);
}