#include "commands/types/looped_command.hpp"
#include "helpers/teleportHelpers.h"
#include "memory/pointers.h"
#include "services/players/player_manager.hpp"

namespace commands
{
	class spectate final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			const auto target_player_ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(g_player_manager->get_selected()->id());
			auto player_position = ENTITY::GET_ENTITY_COORDS(target_player_ped, TRUE);

			if (!target_player_ped)
				return disable();

			g_pointers->g_set_in_spectator_mode(TRUE, target_player_ped);
			NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(FALSE);
			while (!STREAMING::IS_ENTITY_FOCUS(target_player_ped))
			{
				STREAMING::SET_FOCUS_ENTITY(target_player_ped);
				if (teleport::load_ground_at_3d_coordinates(player_position))
				{
					break;
				}
				script::get_current()->yield();
			}
			HUD::SET_MINIMAP_IN_SPECTATOR_MODE(TRUE, target_player_ped);
			HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 0);
		}

		void on_disable() override
		{
			g_pointers->g_set_in_spectator_mode(FALSE, 0);
			NETWORK::NETWORK_OVERRIDE_RECEIVE_RESTRICTIONS_ALL(TRUE);
			HUD::SET_MINIMAP_IN_SPECTATOR_MODE(FALSE, 0);
			HUD::SET_BLIP_ALPHA(HUD::GET_MAIN_PLAYER_BLIP_ID(), 255);
			STREAMING::CLEAR_FOCUS();
			if (!STREAMING::IS_ENTITY_FOCUS(self::ped))
			{
				STREAMING::SET_FOCUS_ENTITY(self::ped);
				STREAMING::SET_RESTORE_FOCUS_ENTITY(self::ped);
			}
		}
	};

	spectate g_spectate("Spectate", "spectate", "Spectate the player");
}
