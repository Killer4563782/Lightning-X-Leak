#include "commands/math.h"
#include "commands/types/looped_command.hpp"

namespace commands
{
	class no_limit final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (self::local_ped)
			{
				self::local_ped->fired_sticky_bombs = 0;
				self::local_ped->fired_flares = 0;
			}
		}
	};

	no_limit g_no_limit("No C4 and Flare limit", "no_limit", "Remove the limitation of how many sticky bombs and flares you can throw");
}
