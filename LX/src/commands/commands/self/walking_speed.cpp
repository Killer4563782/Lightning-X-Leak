#include "commands/types/number/looped_float_command.hpp"

namespace commands
{
	class walking_speed final : looped_float_command
	{
		using looped_float_command::looped_float_command;
		
		void on_tick() override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_player_info->m_run_speed = m_value;
		}

		void on_disable() override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_player_info->m_run_speed = 1.f;
		}
	};

	walking_speed g_walking_speed("Walking Speed", "walking_speed", "Modify Walking Speed", false, false, 1.f, 10.f, 0.5f, 1.f);
}
