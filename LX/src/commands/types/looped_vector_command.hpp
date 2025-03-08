#pragma once
#include "commands/types/bool_command.hpp"

namespace commands
{
	class looped_vector_command ;
	inline std::vector<looped_vector_command *> g_looped_vector_commands;

	class looped_vector_command : public bool_command
	{
	public:
		int m_last_index = 0;
		int m_index;
		std::vector<std::string> m_vector;

		looped_vector_command (const std::string& name, const std::string& command, const std::string& description, bool requires_fiber_pool, bool toggle, 
				const std::vector<std::string>& vector, int starting_index = 0) :
			bool_command(name, command, description, requires_fiber_pool, toggle),
			m_index(starting_index),
			m_vector(vector)
		{
			g_looped_vector_commands.push_back(this);
		}
		

		virtual void on_tick() = 0;
		virtual void on_change(int new_index, int old_index) {}
		virtual void on_reset() override
		{
			disable();
		}

		virtual void refresh() override
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

		virtual void set_index(int index)
		{
			m_index = index;
			refresh();
		}
	};
}
