#include "commands/types/looped_command.hpp"
#include "util/util.h"

namespace commands
{
    class ptfx_gun final : public looped_command
    {
    public:
        using looped_command::looped_command;

    private:
        constexpr static const char* PTFX_ASSET = "scr_powerplay";
        constexpr static const char* PTFX_EFFECT = "sp_powerplay_beast_appear_trails";
        constexpr static float WEAPON_OFFSET_X = 0.7f;
        constexpr static float WEAPON_OFFSET_Y = 0.f;
        constexpr static float WEAPON_OFFSET_Z = 0.f;
        constexpr static float WEAPON_ROTATION_X = 0.f;
        constexpr static float WEAPON_ROTATION_Y = 180.f;
        constexpr static float WEAPON_ROTATION_Z = 0.f;
        constexpr static float WEAPON_SCALE = 1.f;

        void on_tick() override
        {
            // Request the particle effect asset
            if (!util::natives::request_named_ptfx_asset(PTFX_ASSET))
                return;

            // Get the current weapon entity index
            Entity weapon_index = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID(), 0);

            // Get the dimensions of the selected weapon
            Vector3 v0, v1;
            MISC::GET_MODEL_DIMENSIONS(WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID()), &v0, &v1);

            // Calculate the position for the particle effect
            float x_offset = (v0.x - v1.x) / 2.0f + WEAPON_OFFSET_X;

            // Start the particle effect on the weapon bone
            GRAPHICS::USE_PARTICLE_FX_ASSET(PTFX_ASSET);
            GRAPHICS::START_NETWORKED_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(
                PTFX_EFFECT, weapon_index, x_offset, WEAPON_OFFSET_Y, WEAPON_OFFSET_Z,
                WEAPON_ROTATION_X, WEAPON_ROTATION_Y, WEAPON_ROTATION_Z,
                WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE, WEAPON_SCALE
            );
        }

        void on_disable() override
        {
            // Remove the particle effect asset when the command is disabled
            STREAMING::REMOVE_NAMED_PTFX_ASSET(PTFX_ASSET);
        }
    };

    ptfx_gun g_ptfx_gun("PTFX Gun", "ptfx_gun", {});
}