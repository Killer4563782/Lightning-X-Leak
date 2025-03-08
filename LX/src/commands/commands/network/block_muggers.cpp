#include "commands/types/bool_command.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

namespace commands
{
	class block_muggers final : bool_command
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

	block_muggers g_block_muggers("Block Muggers", "block_muggers", "Block other players from sending a mugger on you", false, false);
}
