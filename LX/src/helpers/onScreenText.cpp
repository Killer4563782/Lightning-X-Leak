#include "onScreenText.h"
#include "fiber/script.hpp"
#include "gui/drawing.h"

namespace on_screen_text
{
	void text(const char* text, Vector2 pos, float scale, float size, int r, int g, int b, int a, bool center, int font, float wrap_start, float wrap_end)
	{
		HUD::SET_TEXT_SCALE(scale, size);
		HUD::SET_TEXT_COLOUR(r, g, b, a);
		HUD::SET_TEXT_CENTRE(center);
		HUD::SET_TEXT_FONT(font);
		HUD::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		HUD::SET_TEXT_EDGE(0, 0, 0, 0, 0);
		HUD::SET_TEXT_PROPORTIONAL(1);
		HUD::SET_TEXT_WRAP(wrap_start, wrap_end);
		HUD::SET_TEXT_OUTLINE();
		HUD::BEGIN_TEXT_COMMAND_DISPLAY_TEXT("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text);
		HUD::END_TEXT_COMMAND_DISPLAY_TEXT(pos.x, pos.y - 0.1f, 0);
	}

	void imgui_text(const std::string& text, Vector2 pos, float size, int r, int g, int b, int a, Font font)
	{
		const auto screen_pos = ImVec2(pos.x, pos.y);
		const auto color = ImVec4(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
		drawing::text(font, text, screen_pos, size, color, eJustify::Center, 0.f, 0.f, false);
	}

	void above_map(const std::string& text)
	{
		HUD::SET_TEXT_OUTLINE();
		HUD::BEGIN_TEXT_COMMAND_THEFEED_POST("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.c_str());
		HUD::END_TEXT_COMMAND_THEFEED_POST_MESSAGETEXT_WITH_CREW_TAG_AND_ADDITIONAL_ICON(
			"CHAR_LESTER_DEATHWISH", "CHAR_LESTER_DEATHWISH", true, 5, BRAND, "~c~Notification", 1.f, "___" SHORT_UPPER_BRAND, 5, 0
		);
		HUD::END_TEXT_COMMAND_THEFEED_POST_TICKER(FALSE, FALSE);
	}

	void display_help_text(const std::string& text)
	{
		HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(text.data());
		HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
	}
}
