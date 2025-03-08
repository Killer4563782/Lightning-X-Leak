#pragma once
#include "gui/types.h"

namespace on_screen_text
{
	void text(const char* text, Vector2 pos, float scale, float size, int r, int g, int b, int a, bool center = true, int font = 0, float wrap_start = 0.0f, float wrap_end = 1.0f);
	void imgui_text(const std::string& text, Vector2 pos, float size, int r, int g, int b, int a, ui::Font font);
	void above_map(const std::string& text);
	void display_help_text(const std::string& text);
	void welcome(); 
}