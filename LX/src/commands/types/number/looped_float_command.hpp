#pragma once
#include "commands/types/bool_command.hpp"
#include "commands/math.h"

namespace commands
{
	class looped_float_command;
	inline std::vector<looped_float_command*> g_looped_float_commands;

	class looped_float_command : public bool_command
	{
	public:
		float m_last_value = 0.f;
		float m_value;
		float m_min;
		float m_max;
		float m_step;

		looped_float_command(const std::string& name, const std::string& command, const std::string& description, bool requires_fiber_pool, bool toggle,
				float min, float max, float step = 1.f, float value = 0.f) : bool_command(name, command, description, requires_fiber_pool, toggle),
			m_value(value),
			m_min(min),
			m_max(max),
			m_step(step)
		{
			g_looped_float_commands.push_back(this);
		}

		virtual void on_tick() = 0;
		virtual void on_change(float new_value, float old_value) {}
		virtual void on_reset() override
		{
			disable();
		}
		virtual void refresh() override
		{
			if (!math::are_floats_equal(m_value, m_last_value))
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

		virtual void set_value(float value)
		{
			m_value = value;
			refresh();
		}
	};
}
