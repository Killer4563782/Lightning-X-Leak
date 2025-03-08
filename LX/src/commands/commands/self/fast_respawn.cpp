#include "commands/types/looped_command.hpp"
#include "util/globals.h"

namespace commands
{
	class fast_respawn final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			*scr_globals::disable_wasted_sound.as<bool*>() = true;
			*scr_globals::freemode_properties.at(1761).at(756).as<int*>() |= 1 << 1;
		}

		void on_disable() override
		{
			*scr_globals::disable_wasted_sound.as<bool*>() = false;
			*scr_globals::freemode_properties.at(1761).at(756).as<int*>() &= ~(1 << 1);
		}
	};

	fast_respawn g_fast_respawn("Fast Respawn", "fast_respawn", "Instantly respawn (only online)");
}
