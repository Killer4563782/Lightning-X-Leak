#include "commands/math.h"
#include "commands/types/looped_command.hpp"
#include "helpers/entityHelpers.h"

namespace commands
{
	class cage_gun final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_AIM) && PAD::IS_DISABLED_CONTROL_JUST_PRESSED(0, INPUT_ATTACK))
			{
				Entity entity{};
				if (PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(self::player, &entity) && ENTITY::IS_ENTITY_A_PED(entity))
				{
					entity::cage_ped(entity);
				}
			}
		}
	};

	cage_gun g_cage_gun("Cage Gun", "cage_gun", "Shoot a ped or player to cage");
}
