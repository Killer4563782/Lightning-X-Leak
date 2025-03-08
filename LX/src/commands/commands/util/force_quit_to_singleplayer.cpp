#include "commands/types/command.hpp"

namespace commands
{
	class force_quit_to_singleplayer final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			NETWORK::SHUTDOWN_AND_LOAD_MOST_RECENT_SAVE();
		}
	};

	force_quit_to_singleplayer g_force_quit_to_singleplayer("Force Quit to Singleplayer", "force_quit_to_singleplayer", {}, std::nullopt);
}