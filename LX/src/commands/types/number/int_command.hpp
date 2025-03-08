#pragma once
#include "commands/types/command.hpp"

namespace commands
{
	class int_command;
	inline std::vector<int_command*> g_int_commands;

	class int_command : public command
	{
	public:
		int m_last_value = 0;
		int m_value;
		int m_min;
		int m_max;
		int m_step;

		int_command(const std::string& name, const std::string& command, const std::string& description, bool requires_fiber_pool,
				int min, int max, int step = 1, int value = 0) :
			command(name, command, description, 1, requires_fiber_pool),
			m_value(value),
			m_min(min),
			m_max(max),
			m_step(step)
		{
			g_int_commands.push_back(this);
		}


		virtual void execute(const command_arguments& args) override
		{
			m_value = args.get<int>(0);
			notify(Info, 3s, "'{}' has been set to {}", m_name, m_value);

			this->refresh();
		}

		virtual std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			command_arguments result(1);

			try
			{
				int value = std::stoi(args[0]);
				if (value < m_min || value > m_max)
				{
					notify(Warning, 3s, "Value '{}' is out of range ({} - {})", value, m_min, m_max);
					return std::nullopt;
				}

				result.push(value);
				return result;
			}
			catch (...)
			{
				notify(Warning, 3s, "Value '{}' can not be converted to a number", args[0]);
				return std::nullopt;
			}
		}

		virtual void on_reset() {}
		virtual void on_change(int new_value, int old_value) {}

		virtual void refresh()
		{
			if (m_value != m_last_value)
			{
				m_last_value = m_value;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_change(m_value, m_last_value);
					});
				}
				else on_change(m_value, m_last_value);
			}
		}

		virtual void set_value(int value)
		{
			m_value = value;
			refresh();
		}
	};
}
