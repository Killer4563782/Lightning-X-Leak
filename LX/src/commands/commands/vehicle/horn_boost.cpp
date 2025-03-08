#include "commands/types/looped_vector_command.hpp"

constexpr float horn_boost_speed_increment = 0.5f;
float horn_boost_speed = horn_boost_speed_increment;

namespace commands
{
	class horn_boost final : looped_vector_command 
	{
		using looped_vector_command::looped_vector_command;

		void on_tick() override
		{
			if (!self::vehicle)
			{
				horn_boost_speed = horn_boost_speed_increment;
				return;
			}

			if (PAD::IS_CONTROL_PRESSED(2, INPUT_VEH_HORN))
			{
				switch (m_index)
				{
					case 0:
					{
						const auto vec = ENTITY::GET_ENTITY_FORWARD_VECTOR(self::vehicle);
						ENTITY::APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(self::vehicle, 1, vec.x, vec.y, vec.z, true, false, true, true);
					}
					case 1:
					break;
					{
						if (PAD::IS_CONTROL_JUST_PRESSED(2, INPUT_VEH_HORN))
						{
							horn_boost_speed = ENTITY::GET_ENTITY_SPEED(self::vehicle);
						}

						if (horn_boost_speed < std::numeric_limits<float>::max())
						{
							horn_boost_speed += horn_boost_speed_increment;
						}

						auto entity_coords = ENTITY::GET_ENTITY_COORDS(self::vehicle, TRUE);
						auto offset = Vector3{ 0.f, horn_boost_speed, 0.f };
						auto target_coords = ENTITY::GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(self::vehicle, offset.x, offset.y, offset.z);
						auto velocity = target_coords - entity_coords;

						if (const auto length = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z); length > 0.0f)
						{
							velocity.x = velocity.x / length * horn_boost_speed;
							velocity.y = velocity.y / length * horn_boost_speed;
							velocity.z = velocity.z / length * horn_boost_speed;
						}

						ENTITY::SET_ENTITY_VELOCITY(self::vehicle, velocity.x, velocity.y, velocity.z);
					}
					break;
				}
			}
		}
	};

	horn_boost g_horn_boost("Horn Boost", "horn_boost", "Force - Applies a force to an entity's center of mass, affecting its movement and behavior\nVelocity - Directly sets an entity's speed and direction of motion.", false, false, { "Force", "Velocity" });
}