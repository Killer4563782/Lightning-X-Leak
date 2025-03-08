#include "commands/manager.hpp"
#include "commands/types/number/looped_float_command.hpp"
#include "gui/interface.h"

ControllerInputs freecam_controls[] = { INPUT_LOOK_LR, INPUT_LOOK_UD, INPUT_LOOK_UP_ONLY, INPUT_LOOK_DOWN_ONLY, INPUT_LOOK_LEFT_ONLY, INPUT_LOOK_RIGHT_ONLY, INPUT_LOOK_LEFT, INPUT_LOOK_RIGHT, INPUT_LOOK_UP, INPUT_LOOK_DOWN };
float freecam_multiplier = 0.f;

namespace commands
{
	class freecam final : looped_float_command
	{
		Cam m_camera = -1;

		using looped_float_command::looped_float_command;

		void on_tick() override
		{
			if (!CAM::DOES_CAM_EXIST(m_camera))
			{
				Vector3 pos = g_pointers->g_get_gameplay_cam_coords();
				Vector3 rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
				float fov = CAM::GET_GAMEPLAY_CAM_FOV() + 15.f;
				m_camera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", pos.x, pos.y, pos.z, rot.x, rot.y, rot.z, fov, TRUE, 2);
				CAM::RENDER_SCRIPT_CAMS(true, true, 500, true, true, 0);
			}

			PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			for (const auto& control : freecam_controls)
				PAD::ENABLE_CONTROL_ACTION(0, control, true);

			float multiplier = m_value / 50;
			Vector3 movement = { 0.f, 0.f, 0.f };

			// Up: Left Shift or Dpad Up
			if (handlers::key::move_up)
				movement.z += multiplier / 2;

			// Down: Left Control or Dpad Down
			if (handlers::key::move_down)
				movement.z -= multiplier / 2;

			// Forward: W or Left Stick Up
			if (handlers::key::move_forward)
				movement.y += multiplier;

			// Backward: S or Left Stick Down
			if (handlers::key::move_backward)
				movement.y -= multiplier;

			// Left: A or Left Stick Left
			if (handlers::key::move_left)
				movement.x -= multiplier;

			// Right: D or Left Stick Right
			if (handlers::key::move_right)
				movement.x += multiplier;

			if (movement.x == 0.f && movement.y == 0.f && movement.z == 0.f)
				freecam_multiplier = 0.f;
			else if (freecam_multiplier < 10)
				freecam_multiplier += 0.15f;

			Vector3 pos = CAM::GET_CAM_COORD(m_camera);
			Vector3 rot = CAM::GET_CAM_ROT(m_camera, 2);
			float yaw = math::degrees_to_radians(rot.z);

			pos.x += (movement.x * cos(yaw) - movement.y * sin(yaw)) * freecam_multiplier;
			pos.y += (movement.x * sin(yaw) + movement.y * cos(yaw)) * freecam_multiplier;
			pos.z += movement.z * freecam_multiplier;

			CAM::SET_CAM_COORD(m_camera, pos.x, pos.y, pos.z);
			STREAMING::SET_FOCUS_POS_AND_VEL(pos.x, pos.y, pos.z, 0.f, 0.f, 0.f);

			Vector3 gameplay_cam_rot = CAM::GET_GAMEPLAY_CAM_ROT(2);
			CAM::SET_CAM_ROT(m_camera, gameplay_cam_rot.x, gameplay_cam_rot.y, gameplay_cam_rot.z, 2);
		}

		void on_disable() override
		{
			if (m_camera == -1)
				return;

			CAM::SET_CAM_ACTIVE(m_camera, false);
			CAM::RENDER_SCRIPT_CAMS(false, true, 500, true, true, 0);
			CAM::DESTROY_CAM(m_camera, false);
			STREAMING::CLEAR_FOCUS();
			PAD::ENABLE_ALL_CONTROL_ACTIONS(0);
			m_camera = -1;
		}
	};

	freecam g_freecam("Free Cam", "free_cam", {}, false, false, 0.5f, 50.f, 5.f, 5.f);
}
