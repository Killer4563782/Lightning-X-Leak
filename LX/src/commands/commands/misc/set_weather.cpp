#include "commands/types/vector_command.hpp"

const std::vector<std::string> weathers_display{
	"Extra Sunny",	"Clear",	"Clouds",
	"Smog",			"Foggy",	"Overcast",
	"Rain",			"Thunder",	"Clearing",
	"Neutral",		"Snow",		"Blizzard",
	"Snow Light",	"Xmas",		"Halloween"
};

const std::vector weathers{
	"ExtraSunny",	"Clear",	"Clouds",
	"Smog",			"Foggy",	"Overcast",
	"Rain",			"Thunder",	"Clearing",
	"Neutral",		"Snow",		"Blizzard",
	"SnowLight",	"Xmas",		"Halloween"
};

namespace commands
{
	class set_weather final : vector_command
	{
		using vector_command::vector_command;

		void execute(const command_arguments& args) override
		{
			if (int index = args.get<int>(0); index != static_cast<int>(weathers.size()))
				m_index = index;

			MISC::SET_WEATHER_TYPE_NOW_PERSIST(weathers.at(m_index));
			notify(Info, 3s, "The weather has been set to '{} ({})'", weathers_display.at(m_index), m_index);

			this->refresh();
		}


		std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			command_arguments result(1);

			if (args.empty() || args[0].empty())
			{
				result.push(static_cast<int>(weathers.size()));
				return result;
			}

			try
			{
				int index = std::stoi(args[0]);
				if (index < 0 || index >= static_cast<int>(weathers_display.size()))
				{
					notify(Warning, 3s, "Index '{}' is out of range (0 - {})", args[0], weathers_display.size() - 1);
					return std::nullopt;
				}

				result.push(index);
				return result;
			}
			catch (...) {}

			auto it = std::ranges::find_if(weathers_display, [&args](const std::string& str) { return _stricmp(str.c_str(), join(args, " ").c_str()) == 0; });
			if (it == weathers_display.end())
			{
				notify(Warning, 3s, "Value '{}' is not a valid option and can not be converted to a number", join(args, " "));
				return std::nullopt;
			}

			result.push(it - weathers_display.begin());
			return result;
		}
	};

	set_weather g_set_weather("Set Weather", "set_weather", "Set the local world weather", false, std::nullopt, weathers_display);
}