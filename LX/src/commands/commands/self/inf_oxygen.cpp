#include "commands/types/looped_command.hpp"

namespace commands
{
	class inf_oxygen final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (self::local_ped)
				self::local_ped->m_ped_intelligence->m_oxygen_time = 0.f;
		}
	};

	inf_oxygen g_inf_oxygen("Infinite Oxygen", "inf_oxygen", {});
}
