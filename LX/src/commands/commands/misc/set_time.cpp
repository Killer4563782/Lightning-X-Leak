#include "commands/types/command.hpp"

namespace commands
{
	class set_time final : command
	{
		using command::command;

		void execute(const command_arguments& args) override
		{
			int hours = args.get<int>(0);
			int minutes = args.get<int>(1);
			int seconds = args.get<int>(2);

			NETWORK::NETWORK_OVERRIDE_CLOCK_TIME(hours, minutes, seconds);
		}

		std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			command_arguments result(3);
			if (args.size() < 3)
				return { 0 };

			try
			{
				result.push(std::stoi(args[0]));
				result.push(std::stoi(args[1]));
				result.push(std::stoi(args[2]));
			}
			catch (...)
			{
				notify(Warning, 3s, "Arg could not be converted to a number");
				return std::nullopt;
			}

			return result;
		}
	};

	set_time g_set_time("Set Time", "set_time", "Set the local world time", 3);
}