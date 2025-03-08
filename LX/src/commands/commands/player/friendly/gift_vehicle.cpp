#include "commands/types/player/player_command.hpp"
#include "memory/pointers.h"
#include "services/players/player_manager.hpp"
#include "util/util.h"

namespace commands
{
	class gift_vehicle final : player_command
	{
		using player_command::player_command;

		void execute(const player_ptr player, const command_arguments& args) override
		{
			if (!player || !player->get_net_data() || !player->is_valid())
				return;

			if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), FALSE))
			{
				notify(Error, 8s, "{} is not in any vehicle!", player->get_name());
				return;
			}

			auto get_player_vehicle = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player->id()), FALSE);
			if (!util::natives::request_control(get_player_vehicle))
			{
				notify(Error, 8s, "Failed to take control of {}'s vehicle", player->get_name());
				return;
			}

			if (DECORATOR::DECOR_IS_REGISTERED_AS_TYPE("Player_Vehicle", 3) && DECORATOR::DECOR_EXIST_ON(get_player_vehicle, "Player_Vehicle"))
			{
				notify(Error, 8s, "{} already owns this vehicle!", player->get_name());
				return;
			}

			if (!ENTITY::IS_ENTITY_A_MISSION_ENTITY(get_player_vehicle))
				ENTITY::SET_ENTITY_AS_MISSION_ENTITY(get_player_vehicle, TRUE, TRUE);
			DECORATOR::DECOR_REGISTER("PV_Slot", 3);
			DECORATOR::DECOR_REGISTER("Player_Vehicle", 3);
			DECORATOR::DECOR_SET_BOOL(get_player_vehicle, "IgnoredByQuickSave", FALSE);
			DECORATOR::DECOR_SET_INT(get_player_vehicle, "Player_Vehicle", static_cast<int>(NETWORK::NETWORK_HASH_FROM_PLAYER_HANDLE(player->id())));
			if (VEHICLE::IS_VEHICLE_STOLEN(get_player_vehicle))
				VEHICLE::SET_VEHICLE_IS_STOLEN(get_player_vehicle, FALSE);
			notify(Success, 10s, "Successfully gifted [{} {}] to {}", player->get_vehicle_manufacture_name(), player->get_vehicle_name(), player->get_name());
		}
	};

	gift_vehicle g_gift_vehicle("Gift Vehicle", "gift_vehicle", "Player must have a full garage to gift a vehicle. When gifting, the player must replace an existing vehicle in the garage with the new gifted vehicle. This feature is only available if the player's garage is completely full", 0, false);
}