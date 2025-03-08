#include "commands/types/command.hpp"
#include "helpers/weaponHelpers.h"

namespace commands
{
	class get_all_weapons final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			for (const auto& weapons : weapons::all_weapon_hashes)
			{
				WEAPON::GIVE_DELAYED_WEAPON_TO_PED(self::ped, weapons, 9999, TRUE);
			}
		}
	};

	class remove_all_weapons final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			WEAPON::REMOVE_ALL_PED_WEAPONS(self::ped, FALSE);
		}
	};

	get_all_weapons g_get_all_weapons("Get All Weapons", "get_all_weapons", "You will receive all weapons", std::nullopt, true);
	remove_all_weapons g_remove_all_weapons("Remove All Weapons", "remove_all_weapons", "You will lose all weapons", std::nullopt, true);
}
