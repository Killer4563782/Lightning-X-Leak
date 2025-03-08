#include "commands/types/looped_command.hpp"
#include "util/globals.h"

namespace commands
{
	class off_radar final : looped_command
	{
		using looped_command::looped_command;
		
		void on_tick() override
		{
			MISC::SET_BIT(scr_globals::freemode_global.at(4698).as<int*>(), 2);
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].OffRadarActive = true;
			*scr_globals::freemode_properties.at(58).as<int*>() = NETWORK::GET_NETWORK_TIME() + 1;
		}

		void on_disable() override
		{
			MISC::CLEAR_BIT(scr_globals::freemode_global.at(4698).as<int*>(), 2);
			scr_globals::globalplayer_bd.as<GlobalPlayerBD*>()->Entries[self::player].OffRadarActive = false;
		}
	};

	off_radar g_off_radar("Off Radar", "off_radar", {});
}
