#include "commands/math.h"
#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"

namespace commands
{
	class rapid_fire final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (HUD::IS_PAUSE_MENU_ACTIVE() || PED::IS_PED_DEAD_OR_DYING(self::ped, true) || !PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_ATTACK))
				return;

			const auto weapon_entity = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(self::ped, 0);
			if (!weapon_entity)
				return;

			Vector3 dim_min;
			Vector3 dim_max;
			MISC::GET_MODEL_DIMENSIONS(ENTITY::GET_ENTITY_MODEL(weapon_entity), &dim_min, &dim_max);

			const auto dimensions = (dim_max - dim_min) * 0.5f;
			const auto weapon_position = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(weapon_entity, dimensions.x, 0, dimensions.z * 0.3f);

			auto camera_direction = math::rotation_to_direction(CAM::GET_GAMEPLAY_CAM_ROT(0));
			auto camera_position = g_pointers->g_get_gameplay_cam_coords() + camera_direction;

			auto end = camera_position + camera_direction * 2000.0;

			const auto raycast_handle = SHAPETEST::START_EXPENSIVE_SYNCHRONOUS_SHAPE_TEST_LOS_PROBE(
				camera_position.x, camera_position.y, camera_position.z,
				end.x, end.y, end.z,
				-1, 0, 7
			);

			BOOL did_raycast_hit = FALSE;
			Vector3 raycast_hit_position{};
			Vector3 raycast_surface_normal_hit_position{};
			Entity raycast_hit_entity{};
			SHAPETEST::GET_SHAPE_TEST_RESULT(raycast_handle, &did_raycast_hit, &raycast_hit_position, &raycast_surface_normal_hit_position, &raycast_hit_entity);
			if (did_raycast_hit)
			{
				end = raycast_hit_position;
			}

			Hash weapon_hash;
			WEAPON::GET_CURRENT_PED_WEAPON(self::ped, &weapon_hash, false);

			MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(
				weapon_position.x, weapon_position.y, weapon_position.z,
				end.x, end.y, end.z,
				static_cast<int>(WEAPON::GET_WEAPON_DAMAGE(weapon_hash, 0)), true, weapon_hash,
				self::ped, true, false, -1.0
			);
		}
	};

	rapid_fire g_rapid_fire("Rapid Fire", "rapid_fire", {});
}
