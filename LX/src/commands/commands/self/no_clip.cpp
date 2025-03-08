#include "commands/types/number/looped_float_command.hpp"
#include "gui/interface.h"
#include "util/util.h"
#include <array>

constexpr float NO_CLIP_AIM_SPEED_MULTIPLIER = 0.25f;
constexpr float INITIAL_SPEED = 1.0f;
constexpr float MAX_SPEED_MULTIPLIER = 20.0f;
constexpr float SPEED_INCREMENT = 0.15f;
constexpr int ENTITY_ALPHA = 150;

constexpr std::array CONTROLS = {
    INPUT_SPRINT, INPUT_DUCK, INPUT_MOVE_UP_ONLY, INPUT_MOVE_DOWN_ONLY, INPUT_MOVE_LEFT_ONLY, INPUT_MOVE_RIGHT_ONLY
};

Entity last_entity = {};
float m_speed_multiplier = 0.0f;

namespace commands
{
    class no_clip final : public looped_float_command
    {
    public:
        using looped_float_command::looped_float_command;

    private:
        void on_tick() override
        {
            for (const auto& control : CONTROLS)
                PAD::DISABLE_CONTROL_ACTION(0, control, TRUE);

            const Entity entity = self::vehicle ? self::vehicle : self::ped;

            // Cleanup when entities change
            if (last_entity != entity)
            {
                reset_entity_state(last_entity);
                last_entity = entity;
            }

            Vector3 velocity = calculate_velocity();

            ENTITY::SET_ENTITY_ROTATION(entity, 0.f, CAM::GET_GAMEPLAY_CAM_ROT(2).y, CAM::GET_GAMEPLAY_CAM_ROT(2).z, 2, 0);
            ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(entity, FALSE, FALSE);
            ENTITY::SET_ENTITY_ALPHA(entity, ENTITY_ALPHA, FALSE);

            if (velocity.x == 0.f && velocity.y == 0.f && velocity.z == 0.f)
            {
                ENTITY::FREEZE_ENTITY_POSITION(entity, TRUE);
                m_speed_multiplier = 0.f;
            }
            else
            {
                m_speed_multiplier = std::min(m_speed_multiplier + SPEED_INCREMENT, MAX_SPEED_MULTIPLIER);
                ENTITY::FREEZE_ENTITY_POSITION(entity, FALSE);

                if (!PED::IS_PED_IN_ANY_VEHICLE(self::ped, FALSE))
                    TASK::CLEAR_PED_TASKS_IMMEDIATELY(self::ped);

                if (PAD::IS_DISABLED_CONTROL_PRESSED(0, INPUT_AIM) || CAM::GET_FOLLOW_PED_CAM_VIEW_MODE() == FIRST_PERSON)
                {
                    velocity = velocity * NO_CLIP_AIM_SPEED_MULTIPLIER;
                    const auto offset = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(entity, velocity.x * m_speed_multiplier, velocity.y * m_speed_multiplier, velocity.z * m_speed_multiplier);
                    ENTITY::SET_ENTITY_VELOCITY(entity, 0, 0, 0);
                    ENTITY::SET_ENTITY_COORDS_NO_OFFSET(entity, offset.x, offset.y, offset.z, TRUE, TRUE, TRUE);
                }
                else
                {
                    velocity = velocity * m_value;
                    const auto offset = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(entity, velocity.x, velocity.y, 0.f);
                    velocity.x = offset.x - self::position.x;
                    velocity.y = offset.y - self::position.y;
                    ENTITY::SET_ENTITY_MAX_SPEED(entity, D3D11_FLOAT32_MAX);
                    ENTITY::SET_ENTITY_VELOCITY(entity, velocity.x * m_speed_multiplier, velocity.y * m_speed_multiplier, velocity.z * m_speed_multiplier);
                }
            }
        }

        void on_disable() override
        {
            if (util::natives::request_control(last_entity))
                reset_entity_state(last_entity);
        }

        void reset_entity_state(Entity entity)
        {
            if (entity)
            {
                ENTITY::RESET_ENTITY_ALPHA(entity);
                ENTITY::FREEZE_ENTITY_POSITION(entity, FALSE);
                ENTITY::SET_ENTITY_COMPLETELY_DISABLE_COLLISION(entity, TRUE, FALSE);
            }
        }

        Vector3 calculate_velocity() const
        {
            Vector3 velocity{};
            if (handlers::key::move_up) velocity.z += INITIAL_SPEED / 2;
            if (handlers::key::move_down) velocity.z -= INITIAL_SPEED / 2;
            if (handlers::key::move_forward) velocity.y += INITIAL_SPEED;
            if (handlers::key::move_backward) velocity.y -= INITIAL_SPEED;
            if (handlers::key::move_left) velocity.x -= INITIAL_SPEED;
            if (handlers::key::move_right) velocity.x += INITIAL_SPEED;
            return velocity;
        }
    };

    no_clip g_no_clip("No Clip", "no_clip", "Fly through the map", false, false, 1.f, 50.f, 5.f, 10.f);
}