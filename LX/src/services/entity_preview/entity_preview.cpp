#include "commands/math.h"
#include "fiber/fiber_pool.hpp"
#include "gui/drawing.h"
#include "util/util.h"
#include <helpers/VehicleHelpers.h>

namespace entity_preview
{
	namespace vehicle
	{
		Hash current_hash = 0;
		Hash last_hash = 0;
		Vehicle last_vehicle = 0;
		bool current_max = false;
		bool last_max = false;
		float last_rotation = 0.f;
		int ticks = 0;

		void clean_up()
		{
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(current_hash);

			if (last_vehicle)
				VEHICLE::DELETE_VEHICLE(&last_vehicle);

			current_hash = 0;
			last_hash = 0;
			last_vehicle = 0;
			last_rotation = 0;
			ticks = 0;

			if (g_renderer->m_render_queue.contains("vehicle_preview"))
				g_renderer->m_render_queue.erase("vehicle_preview");
		}

		void preview(Hash vehicle_hash, bool max)
		{
			ticks = 0;

			if (current_hash == vehicle_hash && current_max == max)
				return;

			current_hash = vehicle_hash;
			current_max = max;
			if (!g_open || g_renderer->m_render_queue.contains("vehicle_preview"))
				return;

			g_renderer->m_render_queue["vehicle_preview"] = [&]
			{
				g_fiber_pool->queue_job([&]
				{
					if (!g_open || !g_renderer->m_render_queue.contains("vehicle_preview") || ++ticks > 5 || 
						!util::natives::request_model(current_hash))
						return clean_up();

					if (!last_vehicle || last_hash != current_hash || last_max != current_max)
					{
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(last_hash);
						last_hash = current_hash;
						last_max = current_max;
						if (last_vehicle)
							VEHICLE::DELETE_VEHICLE(&last_vehicle);

						last_vehicle = VEHICLE::CREATE_VEHICLE(current_hash, 0.f, 0.f, 0.f, 0.f, FALSE, FALSE, TRUE);

						if (current_max)
						{
							::vehicle::max_vehicle(last_vehicle, false);
						}
					}

					last_rotation = fmod(last_rotation + 0.5f, 360.f);

					Vector3 minimum, maximum;
					MISC::GET_MODEL_DIMENSIONS(current_hash, &minimum, &maximum);
					Vector3 size = maximum - minimum;

					Vector3 gameplay_camera_direction = math::rotation_to_direction(g_pointers->g_get_final_rendered_cam_rot(0));
					Vector3 gameplay_camera_position = g_pointers->g_get_gameplay_cam_coords() + gameplay_camera_direction * 5.f;
					Vector3 vehicle_spawn_position = gameplay_camera_position + gameplay_camera_direction * size.y;

					ENTITY::FREEZE_ENTITY_POSITION(last_vehicle, TRUE);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(last_vehicle, vehicle_spawn_position.x, vehicle_spawn_position.y, vehicle_spawn_position.z, FALSE, FALSE, FALSE);
					ENTITY::SET_ENTITY_ROTATION(last_vehicle, 0.f, 0.f, last_rotation, 2, TRUE);
					ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(last_vehicle, FALSE, FALSE);
					ENTITY::SET_ENTITY_INVINCIBLE(last_vehicle, TRUE);
					ENTITY::SET_ENTITY_ALPHA(last_vehicle, 220, FALSE);
					ENTITY::SET_ENTITY_CAN_BE_DAMAGED(last_vehicle, FALSE);
					ENTITY::SET_CAN_CLIMB_ON_ENTITY(last_vehicle, FALSE);
				});
			};
		}
	}

	namespace ped
	{
		Hash current_hash = 0;
		Hash last_hash = 0;
		Ped last_ped = 0;
		float last_rotation = 0.f;
		int ticks = 0;

		void clean_up()
		{
			STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(current_hash);

			if (last_ped)
				PED::DELETE_PED(&last_ped);

			current_hash = 0;
			last_hash = 0;
			last_ped = 0;
			last_rotation = 0;
			ticks = 0;

			if (g_renderer->m_render_queue.contains("ped_preview"))
				g_renderer->m_render_queue.erase("ped_preview");
		}

		void preview(Hash ped_hash)
		{
			ticks = 0;

			if (current_hash == ped_hash)
				return;

			current_hash = ped_hash;
			if (!g_open || g_renderer->m_render_queue.contains("ped_preview"))
				return;

			g_renderer->m_render_queue["ped_preview"] = [&]
			{
				g_fiber_pool->queue_job([&]
				{
					if (!g_open || !g_renderer->m_render_queue.contains("ped_preview") || ++ticks > 5 || 
						!util::natives::request_model(current_hash))
						return clean_up();

					if (!last_ped || last_hash != current_hash)
					{
						STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(last_hash);
						last_hash = current_hash;
						if (last_ped)
							PED::DELETE_PED(&last_ped);

						last_ped = PED::CREATE_PED(PED_TYPE_MISSION, current_hash, self::position.x, self::position.y, self::position.z, FALSE, FALSE, FALSE);
						PED::SET_PED_DEFAULT_COMPONENT_VARIATION(last_ped);
					}

					last_rotation = fmod(last_rotation + 0.5f, 360.f);

					Vector3 minimum, maximum;
					MISC::GET_MODEL_DIMENSIONS(current_hash, &minimum, &maximum);
					const Vector3 size = maximum - minimum;

					Vector3 gameplay_camera_direction = math::rotation_to_direction(g_pointers->g_get_final_rendered_cam_rot(0));
					Vector3 gameplay_camera_position = g_pointers->g_get_gameplay_cam_coords() + gameplay_camera_direction;
					const Vector3 ped_spawn_position = gameplay_camera_position + gameplay_camera_direction * size.y * 3.f;

					ENTITY::FREEZE_ENTITY_POSITION(last_ped, TRUE);
					ENTITY::SET_ENTITY_COORDS_NO_OFFSET(last_ped, ped_spawn_position.x, ped_spawn_position.y, ped_spawn_position.z, FALSE, FALSE, FALSE);
					ENTITY::SET_ENTITY_ROTATION(last_ped, 0.f, 0.f, last_rotation, 2, TRUE);
					ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(last_ped, FALSE, FALSE);
					ENTITY::SET_ENTITY_INVINCIBLE(last_ped, TRUE);
					ENTITY::SET_ENTITY_ALPHA(last_ped, 255, FALSE);
					ENTITY::SET_ENTITY_CAN_BE_DAMAGED(last_ped, FALSE);
					ENTITY::SET_CAN_CLIMB_ON_ENTITY(last_ped, FALSE);
				});
			};
		}
	}
};