#include "commands/types/command.hpp"
#include "helpers/entityHelpers.h"

namespace commands
{
	class clean_ped final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			if (!self::local_ped)
				return;

			entity::clean_ped(self::ped);
		}
	};

	clean_ped g_clean_ped("Clean Ped", "clean_ped", "Cleans your player ped");
}
