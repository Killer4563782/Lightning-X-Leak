#pragma once
#include "option.hpp"
#include "helpers/inputs.h"

namespace ui
{
	class color_option final : public option
	{
	public:
		color* m_value;
		bool m_allow_alpha;
		std::function<void()> m_function;

		explicit color_option(std::string name, std::string description, color* value, bool allow_alpha = true, std::function<void()> function = nullptr) : option(std::move(name), std::move(description), OptionType::Color),
			m_value(value),
			m_allow_alpha(allow_alpha),
			m_function(std::move(function))
		{}

		void draw(bool selected) override
		{
			option::draw(selected);
			g_base -= g_options.m_size;

			drawing::rectangle({ g_pos.x + g_width - 0.012f - 0.003f, g_base + g_options.m_size / 2.f - 0.023f / 2 }, { 0.013f, 0.023f }, (m_value->pack() & 0x00FFFFFF) < 0x00FFFFFF / 2 ? color{ 255, 255, 255, 255 } : color{ 0, 0, 0, 255 });
			drawing::rectangle({ g_pos.x + g_width - 0.011f - 0.003f, g_base + g_options.m_size / 2.f - 0.021f / 2 }, { 0.011f, 0.021f }, { m_value->r, m_value->g, m_value->b, m_value->a });

			g_base += g_options.m_size;
		}

		void action(ActionType type, bool first_receiver = true) override
		{
			if (type == ActionType::Enter)
			{
				color_input::get_color(m_name, m_allow_alpha, *m_value, [this](const color& input)
				{
					*m_value = input;
					if (m_function)
						m_function();
				});
			}
		}
	};
}
