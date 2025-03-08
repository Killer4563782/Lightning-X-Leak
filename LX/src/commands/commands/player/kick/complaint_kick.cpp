#include "commands/types/player/player_command.hpp"
#include "rage/util.h"

namespace commands
{
	class complaint_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (!player || !player->is_valid())
				return;

			if (player->is_host())
			{
				notify(Error, "Host is a little bitch! This will backfire at you, try using something else");
				return;
			}

			if (get_network()->m_game_session_ptr->is_host())
			{
				get_network()->m_game_complaint_mgr.raise_complaint(player->get_net_data()->m_host_token);
				return;
			}

			player->trigger_desync_kick = true;

			notify(Info, "Desync has been initiated! This process will take approximately 10 seconds");
		}
	};

	complaint_kick g_complaint_kick("Desync", "desync_kick", "Removes a player through the complaint mechanism without disclosing the sender of the complaint", 0);
}
