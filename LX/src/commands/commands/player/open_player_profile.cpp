#include "commands/types/player/player_command.hpp"
#include "util/util.h"

namespace commands
{
	class open_player_profile final : player_command
	{
		using player_command::player_command;

		void execute(const player_ptr player, const command_arguments& args) override
		{
			if (!player || !player->is_valid() || !player->get_net_data())
				return;

			g_pointers->g_show_profile_by_gamer_handle(&player->get_net_data()->m_gamer_handle);
		}
	};

	open_player_profile g_open_player_profile("Open Profile", "open_player_profile", {}, 0, false);
}