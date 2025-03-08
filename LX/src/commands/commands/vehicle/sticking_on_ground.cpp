#include "commands/types/looped_command.hpp"

namespace commands
{
	class sticking_on_ground final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (self::vehicle)
			{
				if (const float height = ENTITY::GET_ENTITY_HEIGHT_ABOVE_GROUND(self::vehicle); height <= 1.5f)
				{
					VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(self::vehicle, 5.f);
				}
			}
		}
	};

	sticking_on_ground g_sticking_on_ground("Sticking on Ground", "sticking_on_ground", "Keeps the vehicle sticking to the ground");
}
