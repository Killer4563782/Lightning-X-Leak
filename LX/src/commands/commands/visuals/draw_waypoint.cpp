#include "commands/settings.h"
#include "commands/types/looped_command.hpp"
#include "memory/pointers.h"

static void draw_line(const rage::vector4<float>& point, const rage::vector4<float>& next_point)
{
	using namespace commands::settings::visuals;

	GRAPHICS::DRAW_LINE(point.x, point.y, point.z + 1.f, next_point.x, next_point.y, next_point.z + 1.f, waypoint_color.r, waypoint_color.g, waypoint_color.b, waypoint_color.a);
}

namespace commands
{
	class draw_waypoint final : looped_command
	{
		using looped_command::looped_command;

		void on_tick() override
		{
			if (const auto& waypoint_data = g_pointers->g_waypoint_data; waypoint_data && waypoint_data->m_points && waypoint_data->m_count > 1)
			{
				for (uint32_t i = 0; i < waypoint_data->m_count; ++i)
				{
					const auto& point = waypoint_data->m_points[i];
					const auto& next_point = waypoint_data->m_points[i + 1];
					draw_line(point, next_point);
				}
			}
		}
	};

	draw_waypoint g_draw_waypoint("Draw Waypoint", "draw_waypoint", "Draws a line on the road to your waypoint");
}
