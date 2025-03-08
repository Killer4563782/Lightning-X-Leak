#include "commands/math.h"
#include "commands/types/player/player_command.hpp"
#include "services/players/player_manager.hpp"
#include "util/util.h"

namespace commands
{
	class give_health_and_armor final : player_command
	{
		using player_command::player_command;

		void execute(const player_ptr player, const command_arguments& args) override
		{
			if (!player || !player->get_net_data() || !player->is_valid())
				return;

			auto pos = player->get_pos();

			for (auto i = 0; i < 10; i++)
				OBJECT::CREATE_AMBIENT_PICKUP(2406513688, pos->x, pos->y, pos->z + 1.f, 0, 40000, "prop_ld_health_pack"_joaat, false, false);
		}
	};

	give_health_and_armor g_give_health_and_armor("Give Health/Armor", "give_heal_and_armor", "Give health and armor to player", 0, false);
}