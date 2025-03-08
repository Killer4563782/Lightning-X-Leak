#include "commands/types/command.hpp"

namespace commands
{
	class restart_game final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			MISC::RESTART_GAME();
		}
	};

	restart_game g_restart_game("Restart Game", "restart_game", "Restart the game", std::nullopt);
}