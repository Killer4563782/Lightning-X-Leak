#include "visuals.h"

#include "commands/math.h"
#include "helpers/entityHelpers.h"

namespace ui::visuals::bones
{
	void draw_bones()
	{
		if (ImGui::Begin("##Bones", nullptr, 935))
		{
			for (auto& entity : entity::get_entities(false, ped_bones || player_bones, false, false, false, include_self))
			{
				if (!ENTITY::IS_ENTITY_A_PED(entity))
					continue;

				if (auto pos = ENTITY::GET_ENTITY_COORDS(entity, TRUE); math::distance_between_vectors(self::position, pos) > max_distance)
					continue;

				if (PED::GET_PED_TYPE(entity) == PED_TYPE_ANIMAL)
					continue;

				if (entity == PLAYER::PLAYER_PED_ID() && !include_self)
					continue;

				if (PED::IS_PED_A_PLAYER(entity))
				{
					if (!player_bones)
						continue;
				}
				else if (!ped_bones && player_bones && entity != PLAYER::PLAYER_PED_ID()) // not sure if second-third check is needed for story mode or nah
					continue;

				// Lambda to draw a bone with early return for efficiency
				auto draw_bone = [entity](eBone bone1, eBone bone2, const Vector3& offset1 = {}, const Vector3& offset2 = {})
				{
					ImVec2 bone1_pos, bone2_pos;
					if (!get_screen_bone_pos(entity, bone1, &bone1_pos, offset1) || !get_screen_bone_pos(entity, bone2, &bone2_pos, offset2))
					{
						return;
					}

					drawing::line(bone1_pos, bone2_pos, PED::IS_PED_A_PLAYER(entity) ? player_color : ped_color, 1.f, true);
				};

				// Head
				draw_bone(SKEL_Head, SKEL_Neck_1, {0.1f, 0.f, 0.f});

				// Left Arm
				draw_bone(SKEL_Neck_1, SKEL_L_Clavicle);
				draw_bone(SKEL_L_Clavicle, SKEL_L_UpperArm);
				draw_bone(SKEL_L_UpperArm, SKEL_L_Forearm);
				draw_bone(SKEL_L_Forearm, SKEL_L_Hand);

				if (draw_finger_bones)
				{
					draw_bone(SKEL_L_Hand, SKEL_L_Finger00);
					draw_bone(SKEL_L_Finger00, SKEL_L_Finger01);
					draw_bone(SKEL_L_Finger01, SKEL_L_Finger02);

					draw_bone(SKEL_L_Hand, SKEL_L_Finger10);
					draw_bone(SKEL_L_Finger10, SKEL_L_Finger11);
					draw_bone(SKEL_L_Finger11, SKEL_L_Finger12);

					draw_bone(SKEL_L_Hand, SKEL_L_Finger20);
					draw_bone(SKEL_L_Finger20, SKEL_L_Finger21);
					draw_bone(SKEL_L_Finger21, SKEL_L_Finger22);

					draw_bone(SKEL_L_Hand, SKEL_L_Finger30);
					draw_bone(SKEL_L_Finger30, SKEL_L_Finger31);
					draw_bone(SKEL_L_Finger31, SKEL_L_Finger32);

					draw_bone(SKEL_L_Hand, SKEL_L_Finger40);
					draw_bone(SKEL_L_Finger40, SKEL_L_Finger41);
					draw_bone(SKEL_L_Finger41, SKEL_L_Finger42);

					draw_bone(SKEL_R_Hand, SKEL_R_Finger00);
					draw_bone(SKEL_R_Finger00, SKEL_R_Finger01);
					draw_bone(SKEL_R_Finger01, SKEL_R_Finger02);

					draw_bone(SKEL_R_Hand, SKEL_R_Finger10);
					draw_bone(SKEL_R_Finger10, SKEL_R_Finger11);
					draw_bone(SKEL_R_Finger11, SKEL_R_Finger12);

					draw_bone(SKEL_R_Hand, SKEL_R_Finger20);
					draw_bone(SKEL_R_Finger20, SKEL_R_Finger21);
					draw_bone(SKEL_R_Finger21, SKEL_R_Finger22);

					draw_bone(SKEL_R_Hand, SKEL_R_Finger30);
					draw_bone(SKEL_R_Finger30, SKEL_R_Finger31);
					draw_bone(SKEL_R_Finger31, SKEL_R_Finger32);

					draw_bone(SKEL_R_Hand, SKEL_R_Finger40);
					draw_bone(SKEL_R_Finger40, SKEL_R_Finger41);
					draw_bone(SKEL_R_Finger41, SKEL_R_Finger42);
				}

				// Right Arm
				draw_bone(SKEL_Neck_1, SKEL_R_Clavicle);
				draw_bone(SKEL_R_Clavicle, SKEL_R_UpperArm);
				draw_bone(SKEL_R_UpperArm, SKEL_R_Forearm);
				draw_bone(SKEL_R_Forearm, SKEL_R_Hand);

				// Spine
				draw_bone(SKEL_Neck_1, SKEL_Spine3);
				draw_bone(SKEL_Spine3, SKEL_Spine2);
				draw_bone(SKEL_Spine2, SKEL_Spine1);
				draw_bone(SKEL_Spine1, SKEL_Spine0);

				// Left Leg
				draw_bone(SKEL_Spine0, SKEL_L_Thigh);
				draw_bone(SKEL_L_Thigh, SKEL_L_Calf);
				draw_bone(SKEL_L_Calf, SKEL_L_Foot);
				draw_bone(SKEL_L_Foot, SKEL_L_Toe0);

				// Right Leg
				draw_bone(SKEL_Spine0, SKEL_R_Thigh);
				draw_bone(SKEL_R_Thigh, SKEL_R_Calf);
				draw_bone(SKEL_R_Calf, SKEL_R_Foot);
				draw_bone(SKEL_R_Foot, SKEL_R_Toe0);
			}
		}
		ImGui::End();
	}
}
