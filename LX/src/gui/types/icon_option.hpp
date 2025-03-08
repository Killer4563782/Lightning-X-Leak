#pragma once
#include "option.hpp"

namespace ui
{

	class icon_option final : public option
	{
	public:
		std::string m_icon;
		std::function<void()> m_function;

		explicit icon_option(std::string name, std::string description, std::string icon, std::function<void()> function = nullptr) : option(std::move(name), std::move(description), OptionType::Icon),
			m_icon(std::move(icon)),
			m_function(std::move(function))
		{}
#ifndef _DEBUG
		icon_option(std::string name, std::string description, commands::command* cmd, std::string icon) : option(std::move(name), std::move(description), OptionType::Icon),
			m_icon(std::move(icon)),

			m_function([cmd] { constexpr std::vector<std::string> args; cmd->call(args); })
		{}


		icon_option(std::string name, commands::command* cmd, std::string icon) : icon_option(std::move(name), cmd->m_description, cmd, std::move(icon))
		{}

		icon_option(commands::command* cmd, std::string icon) : icon_option(cmd->m_name, cmd, std::move(icon))
		{}
#endif
		void draw(bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			drawing::text(
				g_options.m_font_awesome,
				m_icon,
				{
					g_pos.x + g_width + 0.003f,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_font_awesome, g_options.m_text_size) / 2.f - 0.008f
				},
				0.40f,
				g_options.text_color(selected),
				eJustify::Right,
				-g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		void action(ActionType type, bool first_receiver = true) override
		{
			if (m_function && type == ActionType::Enter)
				m_function();
		}
	};
}