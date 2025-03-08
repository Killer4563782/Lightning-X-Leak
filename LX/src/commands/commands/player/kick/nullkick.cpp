#include "commands/manager.hpp"
#include "commands/types/player/player_command.hpp"
#include "fiber/script.hpp"
#include "util/globals.h"
#include "util/scripts.h"

namespace commands
{
	class null_function_kick : player_command
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& _args) override
		{
			const size_t arg_count = 16;
			int64_t args[arg_count] = { (int64_t)-1638522928, (int64_t)self::local_player, 1 << player->id(), (int64_t)(int)-1 };
			g_pointers->g_trigger_script_event(1, args, arg_count, 1 << player->id(), (int)-1638522928);
		}
	};

	null_function_kick g_null_function_kick("Null kick", "nfkick", "Nullsoutplayer", 0, true);
}
