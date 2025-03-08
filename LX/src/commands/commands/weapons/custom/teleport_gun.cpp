#include "commands/types/looped_command.hpp"

namespace commands
{
	class teleport_gun final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			Vector3 bullet_coord{};
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(self::ped, &bullet_coord))
			{
				PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, bullet_coord.x, bullet_coord.y, bullet_coord.z);
			}
		}
	};

	teleport_gun g_teleport_gun("Teleport Gun", "teleport_gun", "Teleport to where you shoot");
}
