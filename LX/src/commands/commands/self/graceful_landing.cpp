#include "commands/types/looped_vector_command.hpp"

namespace commands
{
	class graceful_landing final : looped_vector_command
	{
		using looped_vector_command::looped_vector_command;
		
		void on_tick() override { }
	};

	graceful_landing g_graceful_landing("Graceful Landing", "graceful_landing", "Land after a fall from very high up like nothing happened", false, false, { "Roll", "Beast" }, 0); // Using vector_command with a "Disabled" option might be better here
}
