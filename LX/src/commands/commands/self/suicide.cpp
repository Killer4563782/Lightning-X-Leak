#include "commands/types/command.hpp"

namespace commands
{
	class suicide final : command
	{
		using command::command;

		void execute(const command_arguments&) override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_health = 0.f;
			//ENTITY::SET_ENTITY_HEALTH(self::ped, 0.f, 0, 0);
		}
	};

	suicide g_suicide("Suicide", "suicide", "Become the emo kid");
}