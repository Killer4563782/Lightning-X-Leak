#include "commands/manager.hpp"
#include "commands/types/player/player_command.hpp"
#include "util/globals.h"
#include "util/scripts.h"

namespace commands
{
	class script_host_kick : commands::player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			if (!player || !player->is_valid())
				return;

			if (!scripts::force_host("freemode"_joaat))
			{
				notify(Error, "Failed to force script host!");
				return;
			}

			*scr_globals::gsbd_kicking.at(player->id(), 1).as<bool*>() = true;
		}
	};

	script_host_kick g_script_host_kick("Secretary", "script_host_kick", "Requesting the host's secretary to remove a player", 0, false);
}
