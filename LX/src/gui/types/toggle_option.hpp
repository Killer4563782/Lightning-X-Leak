#pragma once
#include "option.hpp"
#include "commands/types/bool_command.hpp"

namespace ui
{
	class toggle_option final : public option
	{
	public:
		bool* m_toggle;

		explicit toggle_option(std::string name, std::string description, bool* toggle) : option(std::move(name), std::move(description), OptionType::Toggle),
			m_toggle(toggle)
		{}

		toggle_option(std::string name, std::string description, commands::bool_command* cmd) : option(std::move(name), std::move(description), OptionType::Toggle),
			m_toggle(&cmd->m_toggle)
		{}

		toggle_option(std::string name, commands::bool_command* cmd) : toggle_option(std::move(name), cmd->m_description, cmd)
		{}

		toggle_option(commands::bool_command* cmd) : toggle_option(cmd->m_name, cmd->m_description, cmd)
		{}

		void draw(bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			drawing::text(
				g_options.m_toggle_font,
				*m_toggle ? "a" : "b",
				{
					g_pos.x + g_width + 0.003f,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_toggle_font, g_options.m_text_size) / 2.f
				},
				0.27f,
				g_options.text_color(selected),
				eJustify::Right,
				-g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		void action(ActionType type, bool first_receiver = true) override
		{
			switch (type)
			{
			case ActionType::Enter:
			case ActionType::Left:
			case ActionType::Right:
				*m_toggle ^= 1;
				break;
			}
		}
	};
}
