#pragma once
#include "option.hpp"
#include "commands/types/vector_command.hpp"

namespace ui
{
	class vector_option : public option
	{
	public:
		int* m_index;
		std::vector<std::string>* m_vector;
		std::function<void()> m_function = nullptr;

		explicit vector_option(std::string name, std::string description, int* index, std::vector<std::string>* vector, std::function<void()> function = nullptr) : option(std::move(name), std::move(description), OptionType::Vector),
			m_index(index),
			m_vector(vector),
			m_function(std::move(function))
		{}

		explicit vector_option(std::string name, std::string description, commands::vector_command* cmd) : option(std::move(name), std::move(description), OptionType::VectorBool),
			m_index(&cmd->m_index),
			m_vector(&cmd->m_vector)
		{}

		explicit vector_option(std::string name, commands::vector_command* cmd) : vector_option(std::move(name), cmd->m_description, cmd)
		{}

		explicit vector_option(commands::vector_command* cmd) : vector_option(cmd->m_name, cmd->m_description, cmd)
		{}


		void draw(const bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			drawing::text(
				g_options.m_font,
				selected ? "< " + m_vector->at(*m_index) + " > [ " + std::to_string(*m_index + 1) + " / " + std::to_string(m_vector->size()) + " ]" : m_vector->at(*m_index),
				{
					g_pos.x + g_width,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(g_options.m_font, g_options.m_text_size) / 2.f
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
				case ActionType::Left:
				{
					*m_index = *m_index > 0 ? *m_index - 1 : static_cast<int>(m_vector->size()) - 1;
					break;
				}
				case ActionType::Right:
				{
					*m_index = *m_index >= static_cast<int>(m_vector->size()) - 1 ? 0 : *m_index + 1;
					break;
				}
			default: ;
			}
			
			if (m_function && (type == ActionType::Left || type == ActionType::Right))
				m_function();
		}
	};
}
