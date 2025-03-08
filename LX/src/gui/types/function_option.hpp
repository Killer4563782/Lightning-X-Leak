#pragma once
#include "option.hpp"
#include "commands/types/command.hpp"

namespace ui
{
	class function_option final : public option
	{
	public:
		std::function<void()> m_function;

		explicit function_option(std::string name, std::string description, std::function<void()> function) : option(std::move(name), std::move(description), OptionType::Function),
			m_function(std::move(function))
		{}

		function_option(std::string name, std::string description, commands::command* cmd) : option(std::move(name), std::move(description), OptionType::Function),
			m_function([cmd] { std::vector<std::string> args; cmd->call(args); })
		{}

		function_option(std::string name, commands::command* cmd) : function_option(std::move(name), cmd->m_description, cmd)
		{}

		function_option(commands::command* cmd) : function_option(cmd->m_name, cmd->m_description, cmd)
		{}


		virtual void draw(bool selected) override
		{
			drawing::text(
				g_options.m_font,
				m_name,
				{
					g_pos.x,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_font, g_options.m_break_text_size) / 2.f
				},
				g_options.m_text_size,
				g_options.text_color(selected),
				g_options.m_text_justify,
				g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		virtual void action(ActionType type, bool first_receiver = true) override
		{
			if (type == ActionType::Enter && m_function)
				m_function();
		}
	};
}
