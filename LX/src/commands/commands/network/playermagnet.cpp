#include <commands/types/bool_command.hpp>
#include <commands/settings.h>
#include <hooking/hooking.hpp>

namespace commands
{
	class player_magnet final : bool_command
	{
		using bool_command::bool_command;

		void on_enable() override
		{
			g_fiber_pool->queue_job([this]
			{
				settings::session::player_magnet = true;
			});
		}

		void on_disable() override
		{
			settings::session::player_magnet = false;

			g_fiber_pool->queue_job([this]
			{
				for (const auto& id : session_advertisements)
				{
					rage::netStatus status{};

					if (g_hooking->get_original<hooks::rage_rlScMatchmaking_Unadvertise>()(0, &id, &status))
					{
						status.waitUntilDone();
					}
				}

				session_advertisements.clear();
			});
		}
	};

	player_magnet g_player_magnet("Player Magnet", "player_magnet","Allows you to make a public session and be the host with people in the session");
}
