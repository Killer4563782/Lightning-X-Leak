#include "commands/types/bool_command.hpp"
#include "services/script_patcher/script_patcher_service.hpp"

namespace commands
{
	class block_ceo_raids final : bool_command
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

	block_ceo_raids g_block_ceo_raids("Block CEO Raids", "block_ceo_raids", {}, false, false);
}
