#include "commands/math.h"
#include "commands/types/player/player_command.hpp"
#include "helpers/entityHelpers.h"
#include "helpers/teleportHelpers.h"
#include "util/util.h"

namespace commands
{
	class ram final : player_command 
	{
		using player_command::player_command;

		void execute(player_ptr player, const command_arguments& args) override
		{
			auto ped = player->get_ped();
			if (!ped)
				return;

			g_fiber_pool->queue_job([ped]
			{
				constexpr float spawn_distance = 30.f;
				constexpr float speed = 50.0f;

				auto player_pos = ped->get_position();
				Vector3 spawn_pos = *player_pos;

				{
					float x;
					if (math::random(1.f) > 0.5f) // 50% chance to spawn either -7.5 to -30 or 7.5 to 30
						x = math::random(spawn_distance / 4, spawn_distance);
					else x = math::random(-spawn_distance, -(spawn_distance / 4));
					spawn_pos.x = player_pos->x + x;
					spawn_pos.y = player_pos->y + spawn_distance - x;
				}

				if (Vector3 ground_pos = spawn_pos; teleport::load_ground_at_3d_coordinates(ground_pos))
				{
					if (SYSTEM::VDIST(player_pos->x, player_pos->y, player_pos->z, ground_pos.x, ground_pos.y, ground_pos.z) < spawn_distance * 2.f)
					{
						spawn_pos = ground_pos;
					}
					else spawn_pos.z = std::max(ground_pos.z, spawn_pos.z);
				}
				spawn_pos.z += math::random(3.f, 10.f);



				if (!util::natives::request_model("adder"_joaat))
					return;

				auto vehicle = VEHICLE::CREATE_VEHICLE("adder"_joaat, spawn_pos.x, spawn_pos.y, spawn_pos.z, 0.f, services::is_online(), FALSE, FALSE);
				VEHICLE::SET_VEHICLE_GRAVITY(vehicle, FALSE);

				if (vehicle == 0)
					return;

				auto vehicle_pos = ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);
				Vector3 delta = { player_pos->x - vehicle_pos.x, player_pos->y - vehicle_pos.y, player_pos->z - vehicle_pos.z };

				float yaw = atan2(delta.z, delta.x);
				float pitch = asin(delta.y / sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z));

				ENTITY::SET_ENTITY_ROTATION(vehicle, math::radians_to_degrees(pitch), 0.f, math::radians_to_degrees(yaw) - 90.f, 2, FALSE);


				Vector3 forward_direction = {
					cos(yaw) * cos(pitch),
					sin(pitch),
					sin(yaw) * cos(pitch)
				};

				ENTITY::SET_ENTITY_VELOCITY(vehicle, forward_direction.x * speed, forward_direction.y * speed, forward_direction.z * speed);


				STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED("adder"_joaat);
				ENTITY::SET_ENTITY_AS_NO_LONGER_NEEDED(&vehicle);

				script::get_current()->yield(200ms);

				// Calculate the time it takes for the vehicle to reach the player
				float distance = SYSTEM::VDIST(player_pos->x, player_pos->y, player_pos->z, vehicle_pos.x, vehicle_pos.y, vehicle_pos.z);
				float vehicle_speed = ENTITY::GET_ENTITY_SPEED(vehicle);

				script::get_current()->yield(duration_cast<high_resolution_clock::duration>(duration<float>(distance / vehicle_speed)));
				entity::delete_selected_entity(vehicle, true);
			});
		}
	};

	ram g_ram("Ram", "ram", {}, 0, false);
}
