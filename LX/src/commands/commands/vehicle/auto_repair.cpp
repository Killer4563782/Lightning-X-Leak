#include "commands/types/looped_command.hpp"

namespace commands
{
	class auto_repair final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (self::vehicle)
			{
				if (VEHICLE::GET_DOES_VEHICLE_HAVE_DAMAGE_DECALS(self::vehicle))
					VEHICLE::SET_VEHICLE_FIXED(self::vehicle);

				VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(self::vehicle);

				if (VEHICLE::GET_VEHICLE_ENGINE_HEALTH(self::vehicle) < 1000.f)
					PED::IS_PED_IN_ANY_PLANE(PLAYER::PLAYER_PED_ID()) ? VEHICLE::SET_PLANE_ENGINE_HEALTH(self::vehicle, 1000.f) : VEHICLE::SET_VEHICLE_ENGINE_HEALTH(self::vehicle, 1000.f);
			}
		}
	};

	auto_repair g_auto_repair("Auto Repair", "auto_repair", "It auto repairs your car");
}