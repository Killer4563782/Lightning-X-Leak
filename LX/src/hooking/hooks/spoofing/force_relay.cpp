#include "commands/types/looped_command.hpp"
#include <memory/pointers.h>

namespace commands
{
	class force_relay final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			*g_pointers->g_force_relay_connections = true;
		}
		void on_disable() override {

			*g_pointers->g_force_relay_connections = false;
		}
	};

	force_relay g_force_relay("Force Relay", "force_relay", "Makes it where other people can't see your ip");
}