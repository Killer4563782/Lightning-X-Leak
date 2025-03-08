#pragma once
#include "command.hpp"

namespace commands
{
	class bool_command;
	inline std::vector<bool_command*> g_bool_commands;

	class bool_command : public command
	{
	public:
		bool m_last_enabled = false;
		bool m_toggle;

		bool_command(const std::string& name, const std::string& command, const std::string& description, bool requires_fiber_pool = false, bool toggle = false) : command(name, command, description, std::nullopt, requires_fiber_pool),
			m_toggle(toggle)
		{
			g_bool_commands.push_back(this);
		}


		virtual void execute(const command_arguments& args) override
		{
			m_toggle = args.empty() ? !m_toggle : args.get<bool>(0);
			notify(Info, 3s, "'{}' has been {}", m_name, m_toggle ? "enabled" : "disabled");

			this->refresh();
		}

		virtual std::optional<command_arguments> parse_args(const std::vector<std::string>& args) override
		{
			command_arguments result(1);

			if (args.empty())
				return result;

			if (contains({ "yes", "y", "on", "enable", "true", "t", "1" }, args[0]))
			{
				result.push(true);
				return result;
			}

			if (contains({ "no", "n", "off", "disable", "false", "f", "0" }, args[0]))
			{
				result.push(false);
				return result;
			}

			notify(Warning, 3s, "Value '{}' can not convert to bool", args[0]);
			return std::nullopt;
		}


		virtual void on_reset()
		{
			disable();
		}
		virtual void on_enable() {}
		virtual void on_disable() {}

		virtual void refresh()
		{
			if (m_toggle && !m_last_enabled)
			{
				m_last_enabled = true;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_enable();
					});
				}
				else on_enable();
			}
			else if (!m_toggle && m_last_enabled)
			{
				m_last_enabled = false;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_disable();
					});
				}
				else on_disable();
			}
		}

		virtual void enable()
		{
			if (!m_toggle)
			{
				m_toggle = true;
				m_last_enabled = true;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_enable();
					});
				}
				else on_enable();
			}
		}

		virtual void disable()
		{
			if (m_toggle)
			{
				m_toggle = false;
				m_last_enabled = false;
				if (m_requires_fiber_pool)
				{
					g_fiber_pool->queue_job([this] {
						on_disable();
					});
				}
				else on_disable();
			}
		}

		virtual void toggle()
		{
			m_toggle ? disable() : enable();
		}
	};
}