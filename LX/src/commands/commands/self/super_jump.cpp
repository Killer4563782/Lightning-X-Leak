#include "commands/types/looped_vector_command.hpp"

namespace commands
{
	class super_jump final : looped_vector_command
	{
		using looped_vector_command::looped_vector_command;
		
		void on_tick() override { }
	};

	super_jump g_super_jump("Super Jump", "super_jump", {}, false, false, { "Flag", "Beast" }, 0); // Using vector_command with a "Disabled" option might be better here
}
