#pragma once
#include "option.hpp"
#include "commands/types/number/float_command.hpp"
#include "commands/types/number/int_command.hpp"
#include "helpers/inputs.h"

namespace ui
{
	template <typename T>
	class number_option : public option
	{
	public:
		T* m_value;
		T m_min;
		T m_max;
		T m_step;
		std::function<void()> m_function = nullptr;

		explicit number_option(std::string name, std::string description, T* value, T max, T min = T(0), T step = T(1), std::function<void()> function = nullptr) : option(std::move(name), std::move(description), OptionType::Number),
			m_value(value),
			m_min(min),
			m_max(max),
			m_step(step),
			m_function(std::move(function))
		{}

		explicit number_option(std::string name, std::string description, commands::float_command* cmd) : option(std::move(name), std::move(description), OptionType::Number),
			m_value(&cmd->m_value),
			m_min(cmd->m_min),
			m_max(cmd->m_max),
			m_step(cmd->m_step)
		{}

		explicit number_option(std::string name, commands::float_command* cmd) : number_option(std::move(name), cmd->m_description, cmd)
		{}

		explicit number_option(commands::float_command* cmd) : number_option(cmd->m_name, cmd->m_description, cmd)
		{}

		explicit number_option(std::string name, std::string description, commands::int_command* cmd) : option(std::move(name), std::move(description), OptionType::Number),
			m_value(&cmd->m_value),
			m_min(cmd->m_min),
			m_max(cmd->m_max),
			m_step(cmd->m_step)
		{}

		explicit number_option(std::string name, commands::int_command* cmd) : number_option(std::move(name), cmd->m_description, cmd)
		{}

		explicit number_option(commands::int_command* cmd) : number_option(cmd->m_name, cmd->m_description, cmd)
		{}


		void draw(const bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			std::string value;
			if constexpr (std::is_floating_point<T>())
			{
				value = to_fixed_no_zero(*m_value);
			}
			else
			{
				value = std::to_string(*m_value);
			}

			drawing::text(
				g_options.m_font,
				selected ? "< " + value + " >" : value,
				{
					g_pos.x + g_width,
					g_base + g_options.m_size / 2 - drawing::get_text_height(g_options.m_font, g_options.m_text_size) / 2
				},
				g_options.m_text_size,
				g_options.text_color(selected),
				eJustify::Right,
				-g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		void action(const ActionType type, bool first_receiver = true) override
		{
			switch (type)
			{
			case ActionType::Enter:
			{
				text_input::get_input(m_name, "123456789...", text_input::InputFlags_Allow_Number | text_input::InputFlags_Dont_Add_History, std::to_string(*m_value), [this](const std::string& input)
				{
					if (input.empty())
					{
						*m_value = m_min;
						return;
					}

					if (T num; try_str_to_num(input, num))
					{
						*m_value = std::clamp(num, m_min, m_max);
					}
					else notify(Error, 3s, "The input is not a number.");
				});
				break;
			}
			case ActionType::Left:
			{
				if (*m_value > m_min)
				{
					*m_value = std::clamp(*m_value - m_step, m_min, m_max);
				}
				else *m_value = m_max;

				break;
			}
			case ActionType::Right:
			{
				if (*m_value < m_max)
				{
					*m_value = std::clamp(*m_value + m_step, m_min, m_max);
				}
				else *m_value = m_min;

				break;
			}
			default: ;
			}

			if (m_function && (type == ActionType::Left || type == ActionType::Right))
				m_function();
		}
	};
}
