#pragma once
#include "bool_command.hpp"

namespace commands
{
	class looped_command;
	inline std::vector<looped_command*> g_looped_commands;

	class looped_command : public bool_command
	{
	public:
		looped_command(const std::string& name, const std::string& command, const std::string& description, bool requires_native = false, bool toggle = false) : bool_command(name, command, description, requires_native, toggle)
		{
			g_looped_commands.push_back(this);
		}
		
		virtual void on_tick() = 0;

		void on_reset() override
		{
			disable();
		}
	};
}