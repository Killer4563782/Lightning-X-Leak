#pragma once
#include "gui/drawing.h"
#include "gui/gui.h"

#include "core/curl_client/curl_client.h"

namespace ui
{
	enum class OptionType : uint8_t
	{
		Break,
		Color,
		Function,
		Icon,
		NumberBool,
		Number,
		Option,
		Submenu,
		Toggle,
		VectorBool,
		Vector
	};

	enum class ActionType : uint8_t
	{
		Enter,
		Up,
		Down,
		Left,
		Right
	};

	class option
	{
	public:
		std::string m_name;
		std::string m_description;
		OptionType m_type;

		explicit option(std::string name, std::string description = {}, OptionType option_type = OptionType::Option) : /*m_name(curl_client::translateText(name, "de"))*/m_name(std::move(name)), m_description(std::move(description)), m_type(option_type)
		{
		
		}
		virtual ~option() = default;


		virtual void draw(bool selected)
		{
			drawing::text(
				g_options.m_font,
				m_name,
				{
					g_pos.x,
					g_base + g_options.m_size / 2.f - drawing::get_text_size(g_options.m_font, g_options.m_text_size, m_name).y / 2.f
				},
				g_options.m_text_size,
				g_options.text_color(selected),
				g_options.m_text_justify,
				g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		virtual void action(ActionType type, bool first_receiver = true) {}
	};
}
