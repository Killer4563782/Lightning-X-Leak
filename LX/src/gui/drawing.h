#pragma once
#include "data.h"
#include "grc_textures.h"
#include "types.h"

using namespace ui;

namespace drawing
{
	inline ImGuiIO* g_io;
	inline ImDrawList* g_foreground_drawlist;
	inline ImDrawList* g_background_drawlist;
	inline ImVec2 g_resolution;

	void update();
	ImVec2 cords_to_screen(ImVec2 pos, bool isDC = false);
	ImVec2 get_text_size(Font font_id, float size, const std::string& text, float wrap = -1.f);
	Font get_line_count(const std::string& text, Font font_id, float size, float wrap);

	// Not used commonly, but this is useful for estimating line count, or line size because it gets the text height of a font
	float get_text_height(Font font_id, float size, float wrap = -1.f);
	ImVec2 get_sprite_scale(float size);

	// Pulls a grcTexture from the game, holds the texture shader, texture size and other various data. It holds a high-res copy
	rage::grcTexture* get_game_texture(const sprite& sprite);
	font* get_font(Font font_id);

	void draw_checkmark(ImVec2 pos, color color, float size, bool background = false);
	void rectangle_not_filled(ImVec2 pos, ImVec2 size, color color, float thickness = 1.f, bool background = false, float rounding = 0.f, ImDrawFlags flags = 0);
	void rectangle(ImVec2 pos, ImVec2 size, color color, bool background = false, float rounding = 0.f, ImDrawFlags flags = 0);
	void gradient_rectangle(ImVec2 pos, ImVec2 size, color color_top_left, color color_top_right, color color_bottom_right, color color_bottom_left, bool background = false); 
	void line(ImVec2 start, ImVec2 end, color color, float thickness = 1.f, bool background = false);
	void circle(ImVec2 pos, float radius, color color, bool background = false, float thickness = 1.f, int segments = 12);
	void text(Font font_id, const std::string& text, ImVec2 pos, float size, color color, eJustify justify, float padding, float wrap, bool background = false);
	void gradient_text(Font font_id, const std::string& text, ImVec2 pos, float size, color start_color, color end_color, eJustify justify, float padding, float wrap, bool background = false); 
	void rotated_image(ShaderResource resource, ImVec2 pos, ImVec2 size, color color, float angle);
	void image(ShaderResource resource, ImVec2 pos, ImVec2 size, color color, float rotation = 0.f);
	void image(const sprite& sprite, ImVec2 pos, ImVec2 size, color color);
}
