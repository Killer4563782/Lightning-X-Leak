#include "commands/types/bool_command.hpp"

namespace commands
{
	bool_command g_fast_join("Fast Join", "fast_join", "This will boost your session joining/switching to prevent long pause time in the sky", false, false);
}
