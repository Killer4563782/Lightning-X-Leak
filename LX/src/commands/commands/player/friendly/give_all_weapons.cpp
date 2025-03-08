#include "commands/types/player/player_command.hpp"
#include "helpers/weaponHelpers.h"
#include "memory/pointers.h"
#include "services/players/player_manager.hpp"
#include "util/util.h"

namespace commands
{
	class give_all_weapons final : player_command
	{
		using player_command::player_command;

		void execute(const player_ptr player, const command_arguments& args) override
		{
			if (!player || !player->get_net_data() || !player->is_valid())
				return;

			auto ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id());

			for (const auto& all_weapons : weapons::all_weapon_hashes)
			{
				WEAPON::GIVE_WEAPON_TO_PED(ped, all_weapons, 9999, FALSE, FALSE);
			}
		}
	};

	give_all_weapons g_give_all_weapons("Give All Weapons", "give_all_weapons", "Give all weapons to player", 0, false);
}