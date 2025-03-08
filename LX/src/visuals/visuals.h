#pragma once
#include "gui/data.h"
#include "gui/drawing.h"
#include "imgui/imgui.h"

namespace ui::visuals
{
	// Helpers
	Vector3 get_bone_pos(Entity entity, eBone bone, const Vector3& world_offset);
	bool world_to_screen(const Vector3& world_pos, ImVec2* out);
	bool get_screen_bone_pos(Entity entity, eBone bone, ImVec2* out, const Vector3& world_offset);

	// Main
	void draw_all();

	namespace tracers
	{
		void draw_tracers();

		inline ImVec2 start_pos = { 0.5f, 0.f };
		inline float max_distance = 400.f;
		inline bool use_native_3d = false;
		inline bool include_self = false;

		inline bool player_tracers = false;
		inline color player_color = { 255, 0, 0, 200 };

		inline bool ped_tracers = false;
		inline bool only_humans = true;
		inline color ped_color = { 255, 180, 0, 200 };

		inline bool vehicle_tracers = false;
		inline color vehicle_color = { 0, 255, 0, 200 };

		inline bool prop_tracers = false;
		inline color prop_color = { 0, 0, 255, 100 };
	}

	namespace boxesp
	{
		void draw_boxes();
		
		inline float max_distance = 400.f;
		inline bool include_self = false;

		inline bool player_boxes = false;
		inline color player_color = { 255, 0, 0, 200 };

		inline bool ped_boxes = false;
		inline color ped_color = { 255, 180, 0, 200 };
	}

	namespace bones
	{
		void draw_bones();
		
		inline float max_distance = 400.f;
		inline bool include_self = false;
		inline bool draw_finger_bones = true;

		inline bool player_bones = false;
		inline color player_color = { 255, 0, 0, 200 };

		inline bool ped_bones = false;
		inline color ped_color = { 255, 180, 0, 200 };
	}

	namespace names
	{
		void draw_names();
		
		inline float max_distance = 400.f;
		inline bool include_self = false;
		inline bool show_tags = true;
		inline bool player_names = false;
		inline color player_color = { 255, 255, 255, 255 };
	}
}
