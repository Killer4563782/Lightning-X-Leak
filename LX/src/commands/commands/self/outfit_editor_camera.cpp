#include "commands/settings.h"
#include "commands/types/looped_command.hpp"
#include "helpers/teleportHelpers.h"

namespace commands
{
	class outfit_editor_camera final : looped_command
	{
		using looped_command::looped_command;

		void on_enable() override
		{
			if (!CAM::DOES_CAM_EXIST(teleport::outfit_camera_handle))
			{
				teleport::outfit_camera_handle = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 0);
			}

			//CAM::SET_CAM_ROT(teleport::outfit_camera_handle, 0.0f, 0.0f, ENTITY::GET_ENTITY_HEADING(self::ped), 2);
			CAM::SET_CAM_ACTIVE(teleport::outfit_camera_handle, TRUE);
			CAM::RENDER_SCRIPT_CAMS(TRUE, 1, 1337, 1, 0, 0);
		}

		void on_tick() override
		{
			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			auto in_front_pos = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::ped, 0.0f, 1.f, 0.0f);
			int body_pos = settings::outfit_editor::current_body_pos;

			// Map body_pos to the corresponding bone
			std::array<int, 4> bones = { SKEL_Head, SKEL_Spine3, MH_L_Knee, SKEL_L_Foot };

			// Ensure body_pos is within the valid range
			if (static_cast<size_t>(body_pos) >= static_cast<size_t>(0) && static_cast<size_t>(body_pos) < bones.size())
			{
				int bone = bones[body_pos];
				float z_offset = body_pos == 3 ? 0.1f : 0.0f;  // Add offset for foot

				float z_coords = PED::GET_PED_BONE_COORDS(self::ped, bone, 0.f, 0.f, 0.f).z + z_offset;
				CAM::SET_CAM_COORD(teleport::outfit_camera_handle, in_front_pos.x, in_front_pos.y, z_coords);
				CAM::POINT_CAM_AT_PED_BONE(teleport::outfit_camera_handle, self::ped, bone, 0.f, 0.f, 0.f, TRUE);
			}
		}

		void on_disable() override
		{
			if (CAM::DOES_CAM_EXIST(teleport::outfit_camera_handle))
			{
				CAM::STOP_CAM_POINTING(teleport::outfit_camera_handle);
				CAM::SET_CAM_ACTIVE(teleport::outfit_camera_handle, false);
				CAM::RENDER_SCRIPT_CAMS(false, true, 1337, true, true, 0);
				CAM::DESTROY_CAM(teleport::outfit_camera_handle, false);
			}
		}
	};

	outfit_editor_camera g_outfit_editor_camera("Outfit Editor Camera", "outfit_editor_camera", {});
}
