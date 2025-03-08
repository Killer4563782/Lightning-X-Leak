#include "commands/types/number/looped_float_command.hpp"

namespace commands
{
	class swim_speed final : looped_float_command
	{
		using looped_float_command::looped_float_command;
		
		void on_tick() override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_player_info->m_swim_speed = m_value;
		}

		void on_disable() override
		{
			if (!self::local_ped)
				return;

			self::local_ped->m_player_info->m_swim_speed = 1.f;
		}
	};

	swim_speed g_swim_speed("Swim Speed", "swim_speed", "Modify Swimming Speed", false, false, 1.f, 10.f, 0.5f, 1.f);
}
