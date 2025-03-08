#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"
#include "services/players/player_manager.hpp"

namespace commands
{
	class remove_godmode final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			g_fiber_pool->queue_job([]
			{
				constexpr size_t arg_count = 5;
				int player_id = PLAYER::PLAYER_ID();
				int64_t args[arg_count]{
					-3494809739,
					self::player,
					1 << player_id,
					225624744,
					716
				};

				g_pointers->g_trigger_script_event(1, args, arg_count, 1 << g_player_manager->m_selected_player->id(), -3494809739);
			});
		}
	};

	remove_godmode g_remove_godmode("Remove Godmode", "remove_godmode", "Remove players godmode");
}
