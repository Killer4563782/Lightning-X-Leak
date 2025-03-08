#include "rotaion.h"
#include "imgui/imgui_internal.h"

namespace helpers::rotation
{
	void rotate_start(ImDrawList* draw_list)
	{
		g_index = draw_list->VtxBuffer.Size;
	}

	ImVec2 rotation_center(ImDrawList* draw_list)
	{
		ImVec2 l{ FLT_MAX, FLT_MAX };
		ImVec2 u{ -FLT_MAX, -FLT_MAX };
		const auto& buf = draw_list->VtxBuffer;

		for (int32_t i = g_index; i < buf.Size; ++i)
		{
			l = ImMin(l, buf[i].pos);
			u = ImMax(u, buf[i].pos);
		}
		return (l + u) / 2.f;
	}

	void rotate_end(ImDrawList* draw_list, float rad)
	{
		ImVec2 center = rotation_center(draw_list);
		auto sin = std::sin(rad);
		auto cos = std::cos(rad);
		center = ImRotate(center, sin, cos) - center;
		auto& buf = draw_list->VtxBuffer;

		for (auto i = g_index; i < buf.Size; ++i)
		{
			buf[i].pos = ImRotate(buf[i].pos, sin, cos) - center;
		}
	}
}
