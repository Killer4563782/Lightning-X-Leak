#include "commands/types/looped_command.hpp"
#include "helpers/networkHelpers.h"

namespace commands
{
	class free_movement_in_interior final : looped_command
	{
		using looped_command::looped_command;

		void on_enable() override
		{
			if (network_helpers::is_in_interior(g_player_manager->get_self()))
			{
				PAD::ENABLE_ALL_CONTROL_ACTIONS(0);
			}
		}

		void on_tick() override
		{
			if (network_helpers::is_in_interior(g_player_manager->get_self()))
			{
				// Doesnt seem to work
				PED::SET_PED_CONFIG_FLAG(self::ped, 427, TRUE); // Ignore Interior Check For Sprinting
			}
		}

		void on_disable() override
		{
			if (network_helpers::is_in_interior(g_player_manager->get_self()))
			{
				PED::SET_PED_CONFIG_FLAG(self::ped, 427, FALSE); // Ignore Interior Check For Sprinting
			}
		}
	};

	free_movement_in_interior g_free_movement_in_interior("Free Movement in Interior", "free_movement_in_interior", "Allows you to do some more movements in interiors"); // Allows you to run and jump inside of interiors
}
