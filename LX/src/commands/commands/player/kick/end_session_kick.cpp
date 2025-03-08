#include "commands/manager.hpp"
#include "commands/types/player/player_command.hpp"
#include "fiber/script.hpp"
#include "util/globals.h"
#include "util/scripts.h"

namespace commands
{
	class end_session_kick : player_command
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

			player->trigger_end_session_kick = true;
			*scr_globals::gsbd.as<int*>() = static_cast<int>(__rdtsc() % 50000) + 6;
			script::get_current()->yield(5s);
			*scr_globals::gsbd.as<int*>() = 4;
		}
	};

	end_session_kick g_end_session_kick("Store Closed", "end_session_kick", "Removing a player by reporting them to a store manager", 0, true);
}
