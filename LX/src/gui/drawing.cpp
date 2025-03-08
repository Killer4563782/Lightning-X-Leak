#include "drawing.h"
#include "types.h"
#include "commands/math.h"
#include "helpers/rotaion.h"
#include "imgui/imgui_internal.h"
#include "memory/pointers.h"
#include "renderer/renderer.h"

namespace drawing
{
	void update()
	{
		g_io = &ImGui::GetIO();
		g_resolution = g_io->DisplaySize;
		g_foreground_drawlist = ImGui::GetForegroundDrawList();
		g_background_drawlist = ImGui::GetBackgroundDrawList();
	}

	ImVec2 cords_to_screen(ImVec2 pos, bool isDC)
	{
		const auto scale_factor = isDC ? ImVec2(1.0f, 1.0f) / g_resolution : g_resolution;
		return scale_factor * pos;
	}

	ImVec2 get_text_size(Font font_id, float size, const std::string& text, float wrap)
	{
		if (const auto font = get_font(font_id)->imfont(); font != nullptr)
		{
			auto text_size = font->CalcTextSizeA(font->FontSize * size, FLT_MAX, wrap * g_resolution.x, text.c_str());
			text_size.x = IM_FLOOR(text_size.x + 1.f);
			return cords_to_screen(text_size, true);
		}
		LOG_DEBUG("Failed to get font");
		return { 0.0f, 0.0f };
	}

	Font get_line_count(const std::string& text, Font font_id, float size, float wrap)
	{
		return static_cast<Font>(get_text_size(font_id, size, text, wrap).y / get_text_height(font_id, size, wrap));
	}

	float get_text_height(Font font_id, float size, float wrap)
	{
		return get_text_size(font_id, size, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", wrap).y;
	}

	ImVec2 get_sprite_scale(float size)
	{
		return { g_resolution.y / g_resolution.x * size, size };
	}

	rage::grcTexture* get_game_texture(const sprite& sprite)
	{
		return g_pointers->g_texture_store->find_texture(sprite.m_dictionary, sprite.m_texture);
	}

	font* get_font(Font font_id)
	{
		switch (font_id)
		{
			case Arial: return g_renderer->m_arial;
			case ArialBold: return g_renderer->m_arialBold;
			case ArialItalic: return g_renderer->m_arialItalic;
			case ArialBoldItalic: return g_renderer->m_arialBoldItalic;
			case ArialBlack: return g_renderer->m_arialBlack;
			case Tahoma: return g_renderer->m_tahoma;
			case FontAwesome: return g_renderer->m_fontAwesome;
			case Impact: return g_renderer->m_impact;
			case IconFontData: return g_renderer->m_iconFont;
			case RoboticCyborg: return g_renderer->m_roboticCyborg;
			case Ubuntu: return g_renderer->m_ubuntu;
			default: return g_renderer->m_arial;
		}
	}

	void rectangle_not_filled(ImVec2 pos, ImVec2 size, color color, float thickness, bool background, float rounding, ImDrawFlags flags)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto scaled_pos = cords_to_screen(pos);
		const auto scaled_size = cords_to_screen(size);

		draw_list->AddRect(scaled_pos, scaled_pos + scaled_size, color.pack(), rounding, flags, thickness);
	}

	void rectangle(ImVec2 pos, ImVec2 size, color color, bool background, float rounding, ImDrawFlags flags)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto scaled_pos = cords_to_screen(pos);
		const auto scaled_size = cords_to_screen(size);

		draw_list->AddRectFilled(scaled_pos, scaled_pos + scaled_size, color.pack(), rounding, flags);
	}

	void gradient_rectangle(ImVec2 pos, ImVec2 size, color color_top_left, color color_top_right, color color_bottom_right, color color_bottom_left, bool background)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto scaled_pos = cords_to_screen(pos);
		const auto scaled_size = cords_to_screen(size);

		draw_list->AddRectFilledMultiColor(scaled_pos, scaled_pos + scaled_size, color_top_left.pack(), color_top_right.pack(), color_bottom_right.pack(), color_bottom_left.pack());
	}

	void line(ImVec2 start, ImVec2 end, color color, float thickness, bool background)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto scaled_start = cords_to_screen(start);
		const auto scaled_end = cords_to_screen(end);
		draw_list->AddLine(scaled_start, scaled_end, color.pack(), thickness);
	}

	void circle(ImVec2 pos, float radius, color color, bool background, float thickness, int segments)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto scaled_pos = cords_to_screen(pos);
		const auto scaled_radius = cords_to_screen({ radius, radius }).x;
		draw_list->AddCircle(scaled_pos, scaled_radius, color.pack(), segments, thickness);
	}

	void text(Font font_id, const std::string& text, ImVec2 pos, float size, color color, eJustify justify, float padding, float wrap, bool background)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto font = get_font(font_id)->imfont();

		switch (justify)
		{
			case eJustify::Right:
			{
				const ImVec2 text_size = get_text_size(font_id, size, text, wrap);
				pos.x -= text_size.x;
				break;
			}
			case eJustify::Center:
			{
				const ImVec2 text_size = get_text_size(font_id, size, text, wrap);
				pos.x -= text_size.x / 2.f;
				break;
			}
			default: ;
		}

		const auto scaled_pos = cords_to_screen(pos) + ImVec2{ padding, 0.f };
		draw_list->AddText(font, font->FontSize * size, scaled_pos, color.pack(), text.data(), nullptr, wrap * g_resolution.x);
	}

	color interpolate_color(color start_color, color end_color, float factor)
	{
		const auto r = start_color.r + (end_color.r - start_color.r) * factor;
		const auto g = start_color.g + (end_color.g - start_color.g) * factor;
		const auto b = start_color.b + (end_color.b - start_color.b) * factor;
		const auto a = start_color.a + (end_color.a - start_color.a) * factor;

		return color(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
	}

	void gradient_text(Font font_id, const std::string& text, ImVec2 pos, float size, color start_color, color end_color, eJustify justify, float padding, float wrap, bool background)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		const auto font = get_font(font_id)->imfont();

		switch (justify)
		{
			case eJustify::Right:
			{
				pos.x -= get_text_size(font_id, size, text, wrap).x;
				break;
			}
			case eJustify::Center:
			{
				pos.x -= get_text_size(font_id, size, text, wrap).x / 2.f;
				break;
			}
			default: ;
		}
		const auto scaled_pos = cords_to_screen(pos) + ImVec2{ padding, 0.f };
		const float font_size = font->FontSize * size;

		auto draw_pos = scaled_pos;
		for (size_t i = 0; i < text.size(); ++i)
		{
			char character[2] = { text[i], '\0' };
			auto lerp_factor = static_cast<float>(i) / static_cast<float>(text.size() - 1);
			auto current_color = interpolate_color(start_color, end_color, lerp_factor);
			draw_list->AddText(font, font_size, draw_pos, current_color.pack(), character);
			draw_pos.x += font->CalcTextSizeA(font_size, FLT_MAX, 0.0f, character).x;
		}
	}

	void rotated_image(ShaderResource resource, ImVec2 pos, ImVec2 size, color color, float angle)
	{
		const auto draw_list = g_foreground_drawlist;
		if (angle != 0.f)
		{
			helpers::rotation::rotate_start(draw_list);
		}
		draw_list->AddImage(resource, pos, size, {}, { 1, 1 }, color.pack());
		if (angle != 0.f)
		{
			helpers::rotation::rotate_end(draw_list, math::degrees_to_radians(angle));
		}
	}

	void image(ShaderResource resource, ImVec2 pos, ImVec2 size, color color, float rotation)
	{
		const auto scaled_pos = cords_to_screen(pos);
		const auto scaled_size = cords_to_screen(size);

		rotated_image(resource, scaled_pos, { scaled_pos + scaled_size }, color, rotation);
	}

	void image(const sprite& sprite, ImVec2 pos, ImVec2 size, color color)
	{
		if (const rage::grcTexture* texture = get_game_texture(sprite))
		{
			auto texture_size = texture->size();
			const auto ndc_texture_size = cords_to_screen({ texture_size.x, texture_size.y }, true);
			image(texture->m_shader, pos - ndc_texture_size * size / 2, ndc_texture_size * size, color, sprite.m_rotation);
		}
	}

	void draw_checkmark(const ImVec2& pos, color color, float size, bool background)
	{
		const auto draw_list = background ? g_background_drawlist : g_foreground_drawlist;
		auto scaled_pos = cords_to_screen(pos);

		const auto thickness = (size / 5.0f >= 1.0f ? size / 5.0f : 1.0f);
		size -= thickness * 0.5f;
		scaled_pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

		const auto third = size / 3.0f;
		const auto bx = scaled_pos.x + third;
		const auto by = scaled_pos.y + size - third * 0.5f;
		draw_list->PathLineTo(ImVec2(bx - third, by - third));
		draw_list->PathLineTo(ImVec2(bx, by));
		draw_list->PathLineTo(ImVec2(bx + third * 2.0f, by - third * 2.0f));
		draw_list->PathStroke(ImColor(color), 0, thickness);
	}
}