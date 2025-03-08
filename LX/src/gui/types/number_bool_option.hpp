#pragma once
#include "option.hpp"
#include "commands/types/number/looped_float_command.hpp"
#include "commands/types/number/looped_int_command.hpp"

namespace ui
{
	template <typename T>
	class number_bool_option : public option
	{
	public:
		bool* m_toggle;
		T* m_value;
		T m_min;
		T m_max;
		T m_step;
		std::function<void()> m_function = nullptr;

		explicit number_bool_option(std::string name, std::string description, bool* toggle, T* value, T max, T min = T(0), T step = T(1), std::function<void()> function = nullptr) : option(std::move(name), std::move(description), OptionType::NumberBool),
			m_toggle(toggle),
			m_value(value),
			m_min(min),
			m_max(max),
			m_step(step),
			m_function(std::move(function))
		{}

		explicit number_bool_option(std::string name, std::string description, commands::looped_float_command* cmd) : option(std::move(name), std::move(description), OptionType::NumberBool),
			m_toggle(&cmd->m_toggle),
			m_value(&cmd->m_value),
			m_min(cmd->m_min),
			m_max(cmd->m_max),
			m_step(cmd->m_step)
		{}

		explicit number_bool_option(std::string name, commands::looped_float_command* cmd) : number_bool_option(std::move(name), cmd->m_description, cmd)
		{}

		explicit number_bool_option(commands::looped_float_command* cmd) : number_bool_option(cmd->m_name, cmd->m_description, cmd)
		{}

		explicit number_bool_option(std::string name, std::string description, commands::looped_int_command* cmd) : option(std::move(name), std::move(description), OptionType::NumberBool),
			m_toggle(&cmd->m_toggle),
			m_value(&cmd->m_value),
			m_min(cmd->m_min),
			m_max(cmd->m_max),
			m_step(cmd->m_step)
		{}

		explicit number_bool_option(std::string name, commands::looped_int_command* cmd) : number_bool_option(std::move(name), cmd->m_description, cmd)
		{}

		explicit number_bool_option(commands::looped_int_command* cmd) : number_bool_option(cmd->m_name, cmd->m_description, cmd)
		{}


		void draw(const bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			std::string value;
			if constexpr (std::is_floating_point<T>())
			{
				value = to_fixed_no_zero(*m_value, *m_value < 1.f ? 6 : 2);
			}
			else
			{
				value = std::to_string(*m_value);
			}

			drawing::text(
				g_options.m_font,
				selected ? "< " + value + " >" : value,
				{
					g_pos.x + g_width - 0.008f,
					g_base + g_options.m_size / 2 - drawing::get_text_height(g_options.m_font, g_options.m_text_size) / 2
				},
				g_options.m_text_size,
				g_options.text_color(selected),
				eJustify::Right,
				-g_options.m_padding,
				g_width
			);

			drawing::text(
				g_options.m_toggle_font,
				*m_toggle ? "a" : "b",
				{
					g_pos.x + g_width + 0.003f,
					g_base + g_options.m_size / 2 - drawing::get_text_height(g_options.m_toggle_font, g_options.m_text_size) / 2
				},
				0.27f,
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
				*m_toggle ^= 1;
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
