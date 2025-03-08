#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"

namespace commands
{
	class super_brake final : looped_command
	{
		using looped_command::looped_command;

		uint32_t last_bits = 0;

		void on_tick() override
		{
			if (self::vehicle && PAD::IS_CONTROL_PRESSED(2, ControlVehicleHandbrake))
			{
				ENTITY::SET_ENTITY_VELOCITY(self::vehicle, 0.f, 0.f, 0.f);
			}
		}
	};

	super_brake g_super_brake("Super Brake", "super_brake", "Press hand brake to instantly Brake");
}
