#include "commands/types/player/player_command.hpp"
#include "util/util.h"

namespace commands
{
	class send_friend_request final : player_command
	{
		using player_command::player_command;

		void execute(const player_ptr player, const command_arguments& args) override
		{
			if (!player ||!player->is_valid() || !player->get_net_data())
			{
				notify(Warning, "Player is not valid");
				return;
			}

			g_pointers->g_send_friend_request(&player->get_net_data()->m_gamer_handle, nullptr);
		}
	};

	send_friend_request g_send_friend_request("Send Friend Request", "send_friend_request", {}, 0, false);
}
