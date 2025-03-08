#include "visuals.h"

#include "commands/manager.hpp"
#include "gui/drawing.h"
#include "helpers/entityHelpers.h"
#include "helpers/onScreenText.h"

namespace ui::visuals
{
	// Helper function to convert Vector3 to fvector3
	inline rage::fvector3 to_fvector3(const Vector3& vec)
	{
		return {vec.x, vec.y, vec.z};
	}

	// Get bone position in the world with offset
	Vector3 get_bone_pos(Entity entity, eBone bone, const Vector3& world_offset)
	{
		auto* converted_ped = g_pointers->g_handle_to_pointer(entity);
		rage::fvector3 fworld_offset = to_fvector3(world_offset);
		if (!converted_ped || !g_pointers->g_get_ped_bone_coords(reinterpret_cast<CPed*>(converted_ped), &fworld_offset, bone))
		{
			return {};
		}
		return {fworld_offset.x, fworld_offset.y, fworld_offset.z};
	}

	// Convert world coordinates to screen coordinates
	bool world_to_screen(const Vector3& world_pos, ImVec2* out)
	{
		rage::fvector3 fvector = to_fvector3(world_pos);
		return g_pointers->g_get_screen_coords_from_world_coords(&fvector, &out->x, &out->y);
	}

	// Combined functionality for bone world and screen positions
	bool get_screen_bone_pos(Entity entity, eBone bone, ImVec2* out, const Vector3& world_offset)
	{
		Vector3 bone_pos = get_bone_pos(entity, bone, world_offset);
		return world_to_screen(bone_pos, out);
	}

	void draw_all()
	{
		if (*g_pointers->g_loading_screen_state != eLoadingScreenState::NONE)
			return;

		if (tracers::ped_tracers || tracers::player_tracers || tracers::prop_tracers || tracers::vehicle_tracers)
			tracers::draw_tracers();

		if (boxesp::ped_boxes || boxesp::player_boxes)
			boxesp::draw_boxes();

		if (bones::ped_bones || bones::player_bones)
			bones::draw_bones();

		if (names::player_names)
			names::draw_names();
		 
		if (commands::manager::display_loading_text) 
			on_screen_text::imgui_text("Loading...", { 0.5f, 0.5f }, 1.f, 255, 255, 255, 255, Arial);
		
	}
}