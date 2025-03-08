#include "commands/types/looped_command.hpp"
#include "util/globals.h"

namespace commands
{
	class seamless_joining final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (*scr_globals::transition_state.as<eTransitionState*>() <= eTransitionState::FreemodeFinalSetupPlayer && STREAMING::IS_PLAYER_SWITCH_IN_PROGRESS())
			{
				STREAMING::STOP_PLAYER_SWITCH();
				GRAPHICS::ANIMPOSTFX_STOP_ALL();
			}
		}
	};

	seamless_joining g_seamless_joining("Seamless Joining", "seamless_joining", "Allows you to join sessions and play at the same time");
}