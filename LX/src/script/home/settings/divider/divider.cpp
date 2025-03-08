#include "divider.hpp"

#include "gui/types/color_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/toggle_option.hpp"
#include "gui/types/vector_option.hpp"
#include "gui/types/function_option.hpp"

namespace ui::submenus
{
	void dividers_submenu::on_enter()
	{
		m_subtitle_divider_index = g_subtitle.m_divider_drawing_type;
		m_footer_divider_index = g_footer.m_divider_drawing_type;
	}

	void dividers_submenu::on_tick()
	{
		m_options.clear();
		this->add(vector_option("Subtitle Drawing Type", {}, &m_subtitle_divider_index, &m_drawing_type, [this]
		{
			g_subtitle.m_divider_drawing_type = static_cast<drawing_type>(m_subtitle_divider_index);
		}));

		switch (g_subtitle.m_divider_drawing_type)
		{
			case Static:
			{
				this->add(color_option("Subtitle divider color", {}, &g_subtitle.m_divider_color));
			}
			break;
			case Gradient:
			{
				this->add(color_option("Subtitle divider left color", {}, &g_subtitle.m_divider_color_left));
				this->add(color_option("Subtitle divider right color", {}, &g_subtitle.m_divider_color_right));
			}
			break;
		}

		this->add(vector_option("Footer Drawing Type", {}, &m_footer_divider_index, &m_drawing_type, [this]
		{
			g_footer.m_divider_drawing_type = static_cast<drawing_type>(m_footer_divider_index);
		}));

		switch (g_footer.m_divider_drawing_type)
		{
			case Static:
			{
				this->add(color_option("Footer divider color", {}, &g_footer.m_divider_color));
			}
			break;
			case Gradient:
			{
				this->add(color_option("Footer divider left color", {}, &g_footer.m_divider_color_left));
				this->add(color_option("Footer divider right color", {}, &g_footer.m_divider_color_right));
			}
			break;
		}
	}
}