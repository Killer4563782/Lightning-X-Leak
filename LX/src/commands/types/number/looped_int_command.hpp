#pragma once
#include "commands/types/bool_command.hpp"
#include "commands/math.h"

namespace commands
{
	class looped_int_command;
	inline std::vector<looped_int_command*> g_looped_int_commands;

	class looped_int_command : public bool_command
	{
	public:
		int m_last_value = 0;
		int m_value;
		int m_min;
		int m_max;
		int m_step;

		looped_int_command(const std::string& name, const std::string& command, const std::string& description, bool requires_native, bool toggle,
				int min, int max, int step = 1, int value = 0) : bool_command(name, command, description, requires_native, toggle),
			m_value(value),
			m_min(min),
			m_max(max),
			m_step(step)
		{
			g_looped_int_commands.push_back(this);
		}

		virtual void on_tick() = 0;
		virtual void on_change(int new_value, int old_value) {}
		virtual void on_reset() override
		{
			disable();
		}
		virtual void refresh() override
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

		virtual void set_value(int value)
		{
			m_value = value;
			refresh();
		}
	};
}
