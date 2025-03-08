#include "commands/types/bool_command.hpp"

namespace commands
{
	class console final : bool_command
	{
		using bool_command::bool_command;

		void on_enable() override
		{
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		}

		void on_disable() override
		{
			ShowWindow(GetConsoleWindow(), SW_HIDE);
		}

		void on_reset() override {}
	};

	console g_console("Console", "console", "Toggle the console visibility", false, true);
}
