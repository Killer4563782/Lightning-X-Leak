#include "commands/math.h"
#include "commands/manager.hpp"
#include "commands/types/looped_vector_command.hpp"

namespace commands
{
	class trigger_bot final : looped_vector_command 
	{
		using looped_vector_command::looped_vector_command;

		void on_tick() override
		{
			Entity entity;
			if (!PLAYER::GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(self::player, &entity))
				return;

			if (!ENTITY::IS_ENTITY_A_PED(entity) || PED::IS_PED_DEAD_OR_DYING(entity, false))
				return;

			Vector3 pos{};
			switch (m_index)
			{
				case 0:
				if (PED::IS_PED_A_PLAYER(entity))
					pos = PED::GET_PED_BONE_COORDS(entity, SKEL_Head, 0.1f, 0.f, 0.f);
				break;
				case 1:
				if (!PED::IS_PED_A_PLAYER(entity))
					pos = PED::GET_PED_BONE_COORDS(entity, SKEL_Head, 0.1f, 0.f, 0.f);
				break;
				case 2:
				pos = PED::GET_PED_BONE_COORDS(entity, SKEL_Head, 0.1f, 0.f, 0.f);
				break;
				default:
				break;
			}

			if (pos.x != 0.f || pos.y != 0.f || pos.z != 0.f)
			{
				PED::SET_PED_SHOOTS_AT_COORD(self::ped, pos.x, pos.y, pos.z, true);

				if ("magic_bullets"_BC->m_toggle && PED::IS_PED_SHOOTING(self::ped))
				{
					Hash weapon_hash;
					WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, false);

					const auto ent_pos = ENTITY::GET_ENTITY_COORDS(entity, true);

					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(
						ent_pos.x, ent_pos.y, ent_pos.z,
						pos.x, pos.y, pos.z,
						static_cast<int>(WEAPON::GET_WEAPON_DAMAGE(weapon_hash, 0)), true, weapon_hash,
						self::ped, true, false, -1.0f
					);
				}
			}
		}
	};

	trigger_bot g_trigger_bot("Trigger Bot", "trigger_bot", {}, false, false, { "Players", "Peds", "Players & Peds" });
}