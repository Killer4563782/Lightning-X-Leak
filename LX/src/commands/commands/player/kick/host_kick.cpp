#include "commands/types/player/player_command.hpp"

namespace commands
{
	class host_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (!player || !player->is_valid())
				return;

			if (!g_player_manager->get_self()->is_host())
			{
				notify(Error, "You need to be session host to use this!");
				return;
			}

            NETWORK::NETWORK_SESSION_KICK_PLAYER(player->id());
		}
	};

	host_kick g_host_kick("Host", "host_kick", "Removes a player using host privileges and adds them to a blacklist, preventing further joining", 0, false);
}
