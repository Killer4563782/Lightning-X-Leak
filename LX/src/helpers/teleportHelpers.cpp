#include "teleportHelpers.h"
#include "entityHelpers.h"
#include "vehicleHelpers.h"
#include "commands/manager.hpp"
#include "fiber/fiber_pool.hpp"
#include "fiber/script.hpp"
#include "rage/gta/vehicles.hpp"
#include "util/util.h"

namespace teleport
{
	bool load_ground_at_3d_coordinates(Vector3& location)
	{
	    constexpr int MAX_ATTEMPTS = 200; // Maximum attempts to find ground height
	    constexpr float Z_INCREMENT = 100.f; // Increment for adjusting z-coordinate
	    constexpr float MAX_Z = 1000.f; // Maximum z-coordinate to search
	    commands::manager::display_loading_text = true;

	    float ground_z = location.z;
	    int current_attempts = 0;
	    bool found_ground = false;
	    float water_height;

	    while (current_attempts <= MAX_ATTEMPTS)
	    {
	        // Attempt to get ground height at the current coordinates
	        found_ground = MISC::GET_GROUND_Z_FOR_3D_COORD(location.x, location.y, MAX_Z, &ground_z, false, false);
	        STREAMING::REQUEST_COLLISION_AT_COORD(location.x, location.y, location.z);

	        if (found_ground)
	            break;

	        // Adjust z-coordinate every 10 attempts
	        if (current_attempts % 10 == 0)
	        {
	            if (location.z < MAX_Z)
	            {
	                location.z += Z_INCREMENT;
	            }
	            else
	            {
	                location.z = -200.f;
	            }
	        }

	        ++current_attempts;
	        script::get_current()->yield();
	    }

	    if (!found_ground)
	    {
	        commands::manager::display_loading_text = false;
	        return false;
	    }

	    // Check for water height and adjust z-coordinate accordingly
	    if (WATER::GET_WATER_HEIGHT(location.x, location.y, location.z, &water_height))
	    {
	        location.z = water_height;
	    }
	    else
	    {
	        location.z = ground_z;
	    }

	    commands::manager::display_loading_text = false;
	    return true;
	}

	void set_ped_coords(Ped ped, const Vector3& coords)
	{
		PED::SET_PED_COORDS_KEEP_VEHICLE(ped, coords.x, coords.y, coords.z);
	}

	void teleport_with_animation(const Vector3& coords)
	{
		constexpr int fade_in_time = 1000;
		constexpr int fade_out_time = 500;
		constexpr int cam_creation_time = 1500;
		bool using_vehicle = PED::IS_PED_IN_ANY_VEHICLE(self::ped, false);

		switch (teleport_animation)
		{
		case 0: // Fade
			CAM::DO_SCREEN_FADE_OUT(fade_out_time);
			script::get_current()->yield(718ms);
			set_ped_coords(self::ped, coords);
			CAM::DO_SCREEN_FADE_IN(fade_in_time);
			break;

		case 1: // Swoop Down
			Ped ped_clone;
			ped_clone = PED::CLONE_PED(self::ped, false, false, true);
			ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(ped_clone, false, false);
			set_ped_coords(ped_clone, coords);

			NETWORK::NETWORK_FADE_OUT_ENTITY(using_vehicle ? self::vehicle : self::ped, false, true);

			if (STREAMING::IS_SAFE_TO_START_PLAYER_SWITCH())
			{
				STREAMING::START_PLAYER_SWITCH(self::ped, ped_clone, 0, 1);
			}
			else
			{
				notify(Error, 7s, "Unsafe to start a player switch right now!");
				return;
			}

			while (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS() && STREAMING::GET_PLAYER_SWITCH_STATE() != 9)
			{
				script::get_current()->yield(1ms);
			}

			PED::DELETE_PED(&ped_clone);
			set_ped_coords(self::ped, coords);

			if (STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
			{
				STREAMING::STOP_PLAYER_SWITCH();
			}

			NETWORK::NETWORK_FADE_IN_ENTITY(using_vehicle ? self::vehicle : self::ped, true, 0);
			STREAMING::SET_RESTORE_FOCUS_ENTITY(self::ped);
			break;

		case 2: // Smooth
			if (!CAM::DOES_CAM_EXIST(camera_handle))
			{
				camera_handle = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", false);
			}

			CAM::SET_CAM_COORD(camera_handle, coords.x, coords.y, coords.z + 1000.f);
			CAM::SET_CAM_ACTIVE(camera_handle, true);
			CAM::RENDER_SCRIPT_CAMS(true, true, cam_creation_time, true, false, false);
			CAM::POINT_CAM_AT_COORD(camera_handle, coords.x, coords.y, coords.z);

			script::get_current()->yield(1718ms);

			set_ped_coords(self::ped, coords);

			if (CAM::DOES_CAM_EXIST(camera_handle))
			{
				CAM::STOP_CAM_POINTING(camera_handle);
				CAM::SET_CAM_ACTIVE(camera_handle, false);
				CAM::RENDER_SCRIPT_CAMS(false, true, cam_creation_time, true, true, false);
				CAM::DESTROY_CAM(camera_handle, false);
			}

			STREAMING::SET_RESTORE_FOCUS_ENTITY(self::ped);
			break;

		default:
			break;
		}
	}

	void teleport_to_waypoint()
	{
		g_fiber_pool->queue_job([&]
		{
			util::blip way_point{BlipSpriteWaypoint, util::eBlipHandleType::first};
			if (!way_point.exists()) return;

			auto coords = way_point.coords();
			const bool found_collision = load_ground_at_3d_coordinates(coords);

			if (!found_collision && !force_teleport_when_no_collision_found)
			{
				notify(Error, "Failed to find ground at waypoint");
				return;
			}

			if (const auto interior = INTERIOR::GET_INTERIOR_AT_COORDS(coords.x, coords.y, coords.z); check_for_interior_collision && INTERIOR::IS_VALID_INTERIOR(interior))
			{
				constexpr int max_attempts = 100;
				INTERIOR::PIN_INTERIOR_IN_MEMORY(interior);

				for (int attempts = 0; attempts < max_attempts; attempts++)
				{
					if (INTERIOR::IS_INTERIOR_READY(interior)) break;
					script::get_current()->yield();
				}

				if (!INTERIOR::IS_INTERIOR_READY(interior))
				{
					INTERIOR::UNPIN_INTERIOR(interior);
					notify(Error, "Failed to load interior");
					return;
				}

				for (int attempts = 0; attempts < max_attempts; attempts++)
				{
					if (!INTERIOR::IS_COLLISION_MARKED_OUTSIDE(coords.x, coords.y, coords.z))
					{
						if (using_teleport_animation) coords.z += 2.f;
						break;
					}
					coords.z -= 2.f;
					script::get_current()->yield();
				}

				if (INTERIOR::IS_COLLISION_MARKED_OUTSIDE(coords.x, coords.y,using_teleport_animation ? coords.z - 2.f : coords.z))
				{
					INTERIOR::UNPIN_INTERIOR(interior);
					notify(Error, "Failed to find ground location of interior");
					return;
				}

				INTERIOR::UNPIN_INTERIOR(interior);
			}

			const float final_z = check_for_interior_collision ? coords.z + 0.5f : coords.z;
			if (!using_teleport_animation)
			{
				set_ped_coords(self::ped, { coords.x, coords.y, !found_collision && force_teleport_when_no_collision_found ? coords.z + 1000.f : final_z });
			}
			else
			{
				teleport_with_animation({ coords.x, coords.y,!found_collision && force_teleport_when_no_collision_found ? coords.z + 1000.f : final_z });
			}
		});
	}

	void teleport_to_objective()
	{
		g_fiber_pool->queue_job([]
		{
			// Check if the objective blip exists
			util::blip objective{BlipColorMission, util::eBlipHandleType::first};
			if (!objective.exists())
			{
				notify(Error, "You don't have an objective");
				return;
			}

			// Retrieve objective coordinates
			auto coords = objective.coords();

			// Load ground collision at the objective coordinates
			if (!load_ground_at_3d_coordinates(coords))
			{
				notify(Error, "Failed to find ground location");
				return;
			}

			// Teleport directly if no animation is used
			if (!using_teleport_animation)
			{
				PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, coords.x, coords.y, coords.z + 1.f);
				return;
			}

			// Use teleport animation if specified
			teleport_with_animation(coords);
		});
	}

	// Network related
	void to_coords(const Vector3& location, const Vector3& euler = { 0, 0, 0 })
	{
		PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, location.x, location.y, location.z + 1.f);

		if (euler.x != 0.f)
		{
			if (PED::IS_PED_IN_ANY_VEHICLE(self::ped, true))
			{
				ENTITY::SET_ENTITY_HEADING(self::vehicle, euler.x);
			}
			else
			{
				ENTITY::SET_ENTITY_HEADING(self::ped, euler.x);
			}
		}

		if (euler.y != 0.f || euler.z != 0.f)
		{
			CAM::SET_GAMEPLAY_CAM_RELATIVE_PITCH(euler.y, 1.f);
			CAM::SET_GAMEPLAY_CAM_RELATIVE_HEADING(euler.z);
		}
	}

	bool teleport_player_to_coords(const player_ptr& player, Vector3 coords, const Vector3& euler)
	{
		Entity ent = services::is_online()
			             ? PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id())
			             : PLAYER::PLAYER_PED_ID();

		if (ent == self::ped)
		{
			to_coords(coords, euler);
			return true;
		}

		if (ENTITY::IS_ENTITY_DEAD(ent, true))
		{
			notify(Error, 5s, "{} is dead", player->get_name());
			return false;
		}

		if (PED::IS_PED_IN_ANY_VEHICLE(ent, true))
		{
			ent = PED::GET_VEHICLE_PED_IS_IN(ent, false);

			if (util::natives::forcefully_take_control(ent))
			{
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(ent, coords.x, coords.y, coords.z + 1.f, TRUE, TRUE, TRUE);
				if (euler.x != 0.f)
				{
					ENTITY::SET_ENTITY_HEADING(ent, euler.x);
				}
			}
			else
			{
				notify(Error, 5s, "Failed to get control of the vehicle");
				return false;
			}

			return true;
		}

		auto hnd = vehicle::spawn_vehicle_for_teleporting(VEHICLE_ZENTORNO, *player->get_ped()->get_position(),
			ENTITY::GET_ENTITY_HEADING(ent), true, false);

		if (!hnd) 
			return false;

		if (!g_pointers->g_handle_to_pointer(hnd)->m_net_object) 
			return false;

		ENTITY::SET_ENTITY_VISIBLE(hnd, FALSE, FALSE);
		ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(hnd, FALSE, FALSE);
		ENTITY::FREEZE_ENTITY_POSITION(hnd, TRUE);

		const auto obj_id = player->get_ped()->m_net_object->m_object_id;
		auto veh_id = g_pointers->g_handle_to_pointer(hnd)->m_net_object->m_object_id;
		remote_player_teleport remote_tp = { obj_id, {coords.x, coords.y, coords.z} };

		g_remote_player_teleports.emplace(veh_id, remote_tp);

		if ((player->is_valid() && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), false))
			|| PLAYER::IS_REMOTE_PLAYER_IN_NON_CLONED_VEHICLE(player->id()))
		{
			g_pointers->g_clear_ped_task_network(player->get_ped(), true);
		}

		for (int i = 0; i < 50; ++i)
		{
			script::get_current()->yield(25ms);

			if (const auto ptr = g_pointers->g_handle_to_pointer(hnd))
			{
				if (const auto net_obj = ptr->m_net_object)
				{
					g_pointers->g_migrate_object(player->m_net_game_player, net_obj, 3);
				}
			}

			if (const auto new_coords = ENTITY::GET_ENTITY_COORDS(hnd, true); SYSTEM::VDIST2(
				coords.x, coords.y, coords.z, new_coords.x, new_coords.y,
				new_coords.z) < 20 * 20 && VEHICLE::GET_PED_IN_VEHICLE_SEAT(hnd, 0, true) == ent)
			{
				break;
			}
		}

		std::erase_if(g_remote_player_teleports, [veh_id](auto& obj)
		{
			return obj.first == veh_id;
		});

		entity::delete_selected_entity(hnd);

		return true;
	}

	void teleport_to_player(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			"spectate"_LC->enable();
			script::get_current()->yield(1337ms);
			const auto player_coords = player->get_ped()->m_navigation->get_position();
			PED::SET_PED_COORDS_KEEP_VEHICLE(self::ped, player_coords->x, player_coords->y, player_coords->z + 1.f);
			"spectate"_LC->disable();
		});
	}

	bool teleport_player_to_me(const player_ptr& player)
	{
		"spectate"_LC->enable();
		script::get_current()->yield(1337ms);
		if (teleport_player_to_coords(player, self::position))
		{
			"spectate"_LC->disable();
			return true;
		}
		"spectate"_LC->disable();
		return false;
	}

	void teleport_into_player_vehicle(const player_ptr& player)
	{
		g_fiber_pool->queue_job([&]
		{
			"spectate"_LC->enable();
			script::get_current()->yield(1337ms);
			if (const auto player_vehicle = player->get_current_vehicle())
			{
				const auto handle = g_pointers->g_pointer_to_handle(player_vehicle);
				vehicle::into_vehicle(handle);
				"spectate"_LC->disable();
				return;
			}
			notify(Error, "{} is not in any vehicle", player->get_name());
			"spectate"_LC->disable();
		});
	}

	bool teleport_player_to_way_point(const player_ptr& player)
	{
		util::blip way_point{ BlipSpriteWaypoint, util::eBlipHandleType::first };
		if (!way_point.exists())
		{
			notify(Error, "Failed to find a way point!");
			return false;
		}
		"spectate"_LC->enable();
		script::get_current()->yield(1337ms);
		if (load_ground_at_3d_coordinates(way_point.coords()) && teleport_player_to_coords(player, way_point.coords()))
		{
			script::get_current()->yield(1337ms);
			"spectate"_LC->disable();
			return true;
		}
		"spectate"_LC->disable();
		return false;
	}
}
