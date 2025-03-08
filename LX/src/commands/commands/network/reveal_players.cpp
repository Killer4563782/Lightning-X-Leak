#include "commands/types/bool_command.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

namespace commands
{
	class reveal_players final : bool_command
	{
		using bool_command::bool_command;

		void on_enable() override
		{
			g_script_patcher_service->update();
		}

		void on_disable() override
		{
			g_script_patcher_service->update();
		}
	};

	reveal_players g_reveal_players("Reveal Players", "reveal_players", "Reveal off radar, undead off radar, hiding from player list and invisible players");
}
