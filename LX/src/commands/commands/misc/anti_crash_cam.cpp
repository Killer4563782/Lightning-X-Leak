#include "commands/types/looped_command.hpp"

namespace commands
{
	class anti_crash_cam final : looped_command
	{
		using looped_command::looped_command;

		Cam anti_crash_camera = -1;

		void on_tick() override
		{
			if (!CAM::DOES_CAM_EXIST(anti_crash_camera))
			{
				anti_crash_camera = CAM::CREATE_CAM_WITH_PARAMS("DEFAULT_SCRIPTED_CAMERA", 7000.f, 2000.f, 1000.f, 0.f, 0.f, 0.f, 100.f, TRUE, 2);
				CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE, 0);
			}
		}

		void on_disable() override
		{
			if (anti_crash_camera == -1)
				return;

			CAM::SET_CAM_ACTIVE(anti_crash_camera, FALSE);
			CAM::RENDER_SCRIPT_CAMS(FALSE, FALSE, 0, TRUE, TRUE, 0);
			CAM::DESTROY_CAM(anti_crash_camera, FALSE);
			anti_crash_camera = -1;
		}
	};

	anti_crash_cam g_anti_crash_cam("Anti Crash Camera", "anti_crash_cam", "Prevents loading things around your player, sets the camera very high up");
}
