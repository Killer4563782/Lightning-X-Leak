#include "header.h"

#include "custom_header.h"
#include "gui/types//color_option.hpp"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/number_option.hpp"
#include "gui/types/vector_option.hpp"
#include "helpers/imageLoaderHelpers.h"

namespace ui::submenus
{
	void header_submenu::on_enter()
	{
		m_font_index = g_header.m_font;
	}

	void header_submenu::on_tick()
	{
		m_options.clear();
		this->add(custom_header_submenu("Custom Header"));
		this->add(function_option("Load Default Header", {}, [this]
		{
			if (!image_loader::has_header_loaded || texture_loading_in_progress)
			{
				notify(Info, "The default header already in use");
				return;
			}

			image_loader::has_header_loaded = false;
			image_loader::m_header.clear();
			image_loader::m_header_frame = 0;
		}));
		this->add(function_option("Reset to Default", {}, [this]
		{
			g_header.m_color = color(51, 108, 184, 255);
			g_header.m_text_color = color(255, 255, 255, 255);
			m_font_index = RoboticCyborg;
			g_header.m_font = RoboticCyborg;
			g_header.m_text_size = 1.2f;
			g_header.m_size = 0.1f;
		}));
		this->add(break_option("Modifier"));
		this->add(number_option("Header height", "changes the background height of the header", &g_header.m_size, 0.5f, 0.1f, 0.01f));
		this->add(number_option("Font Size", "Change the size of the header font", &g_header.m_text_size, 1.2f, 0.1f, 0.1f));
		this->add(vector_option("Header Font", "Change the font of the header", &m_font_index, &font_names, [this]
		{
			g_header.m_font = static_cast<Font>(m_font_index);
		}));
	}
}