#include "commands/types/looped_command.hpp"

namespace commands
{
	class no_ragdoll final : looped_command
	{
		using looped_command::looped_command;
		
		void on_tick() override
		{
			PED::SET_PED_CAN_RAGDOLL(self::ped, false);
		}

		void on_disable() override
		{
			PED::SET_PED_CAN_RAGDOLL(self::ped, true);
		}
	};

	no_ragdoll g_no_ragdoll("No Ragdoll", "no_ragdoll", {});
}
