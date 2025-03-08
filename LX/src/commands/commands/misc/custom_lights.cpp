#include "commands/types/looped_command.hpp"
#include "commands/settings.h"

namespace commands
{
	using namespace settings::custom_lights;

	class custom_lights final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			GRAPHICS::DRAW_LIGHT_WITH_RANGE(self::position.x, self::position.y, self::position.z, color.r, color.g, color.b, range, intensity);
		}
	};

	custom_lights g_custom_lights("Custom Lights", "custom_lights", "Locally draws lights around you");
}
