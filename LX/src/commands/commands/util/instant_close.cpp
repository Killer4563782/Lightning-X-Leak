#include "commands/types/looped_command.hpp"

namespace commands
{
	class instant_close final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (GetAsyncKeyState(VK_MENU) && GetAsyncKeyState(VK_F4))
				TerminateProcess(GetCurrentProcess(), 0);
		}
	};

	instant_close g_instant_close("Instant Close", "instant_close", "Instantly close the game by pressing ALT + F4");
}