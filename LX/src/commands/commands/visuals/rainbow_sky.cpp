#include "commands/types/looped_command.hpp"
#include "commands/settings.h"

namespace commands
{
	class rainbow_sky final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			//fade_rgb(commands::settings::sky_rgb);
		}
	};

	rainbow_sky g_rainbow_sky("Rainbow Sky", "rainbow_sky", "make the sky rainbow");
}
