#include "commands/types/player/player_command.hpp"
#include "script/home/network/players/player_kick/player_kick.h"

namespace commands
{
	class smart_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (g_player_manager->get_self()->is_host())
			{
				dynamic_cast<player_command*>(get("host_kick"))->call(player, {});
			}
			else
			{
				if (player->is_host())
				{
					dynamic_cast<player_command*>(get("oom_kick"))->call(player, {});
				}
				else
				{
					dynamic_cast<player_command*>(get("desync_kick"))->call(player, {});
				}
			}
		}
	};

	smart_kick g_smart_kick("Smart", "smart_kick", "Removing a player discreetly without being caught red-handed", 0, false);
}
