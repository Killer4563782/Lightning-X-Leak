#include "commands/types/looped_command.hpp"
#include "gui/interface.h"

namespace commands
{
	class disable_input final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(2);
		}

		void on_disable() override
		{
			auto tick_count = GetTickCount64() + 300;
			handlers::key::open.m_last_down = tick_count;
			handlers::key::back.m_last_down = tick_count;
			handlers::key::enter.m_last_down = tick_count;
			handlers::key::up.m_last_down = tick_count;
			handlers::key::down.m_last_down = tick_count;
			handlers::key::left.m_last_down = tick_count;
			handlers::key::right.m_last_down = tick_count;

			PAD::ENABLE_ALL_CONTROL_ACTIONS(2);
		}
	};

	disable_input g_disable_input("Disable Input", "disable_input", "This command is automatically managed by the menu", false, false);
}
