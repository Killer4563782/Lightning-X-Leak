#include "commands/types/player/player_command.hpp"
#include "rage/util.h"

namespace commands
{
	class breakup_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (!player || !player->get_net_data() || !player->is_valid())
				return;

			if (!g_player_manager->get_self()->is_host())
			{
				notify(Error, "You must be session host to use this!");
				return;
			}

			rage::snMsgRemoveGamersFromSessionCmd cmd{};
			cmd.m_session_id = get_network()->m_game_session_ptr->m_rline_session.m_session_id;
			cmd.m_num_peers = 1;
			cmd.m_handles[0] = player->get_net_data()->m_gamer_handle;
			cmd.m_unk = 19;

			g_pointers->g_handle_remove_gamer_cmd(get_network()->m_game_session_ptr, player->get_session_player(), &cmd);
		}
	};

	breakup_kick g_breakup_kick("Ban", "break_up_kick", "Utilizes host privileges to remove a player, showing a 'cheating message' but refrains from adding the player to a blacklist", 0, false);
}
