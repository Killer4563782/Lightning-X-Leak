#pragma once
#include "commands/types/command.hpp"

namespace commands
{
	class vector_command;
	inline std::vector<vector_command*> g_vector_commands;

	class vector_command : public command
	{
	public:
		int m_last_index = 0;
		int m_index;
		std::vector<std::string> m_vector;

		vector_command(const std::string& name, const std::string& command, const std::string& description, bool requires_fiber_pool, std::optional<uint8_t> num_args, 
				const std::vector<std::string>& vector, int starting_index = 0) :
			command(name, command, description, num_args, requires_fiber_pool),
			m_index(starting_index),
			m_vector(vector)
		{
			g_vector_commands.push_back(this);
		}


		virtual void execute(const command_arguments& args) override
		{
			m_index = args.get<int>(0);
			notify(Info, 3s, "'{}' has been set to '{} ({})'", m_name, m_vector.at(m_index), m_index);

			this->refresh();
		}

		virtual std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			command_arguments result(1);

			try
			{
				int index = std::stoi(args[0]);
				if (index < 0 || index >= m_vector.size())
				{
					notify(Warning, 3s, "Index '{}' is out of range (0 - {})", args[0], m_vector.size() - 1);
					return std::nullopt;
				}

				result.push(index);
				return result;
			}
			catch (...) {}

			auto it = std::ranges::find_if(m_vector, [&args](const std::string& str) { return _stricmp(str.c_str(), join(args, " ").c_str()) == 0; });
			if (it == m_vector.end())
			{
				notify(Warning, 3s, "Value '{}' is not a valid option and can not be converted to a number", join(args, " "));
				return std::nullopt;
			}

			result.push(it - m_vector.begin());
			return result;
		}

		virtual void on_reset() {}
		virtual void on_change(int new_index, int old_index) {}

		virtual void refresh()
		{
			if (m_index != m_last_index)
			{
				m_last_index = m_index;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_change(m_index, m_last_index);
					});
				}
				else on_change(m_index, m_last_index);
			}
		}

		virtual void set_index(int index)
		{
			m_index = index;
			refresh();
		}
	};
}
