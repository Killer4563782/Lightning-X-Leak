#pragma once

namespace helpers::rotation
{
	inline int32_t g_index;

	void rotate_start(ImDrawList* draw_list);
	ImVec2 rotation_center(ImDrawList* draw_list);
	void rotate_end(ImDrawList* draw_list, float rad);
}