#pragma once
#include "services/players/player_manager.hpp"

namespace teleport
{
	inline Cam camera_handle{};
	inline Cam outfit_camera_handle{};
	bool load_ground_at_3d_coordinates(Vector3& location);
	void teleport_with_animation(const Vector3& coords);
	void teleport_to_waypoint();
	void teleport_to_objective();
	//Network related
	void to_coords(const Vector3& location, const Vector3& euler);
	bool teleport_player_to_coords(const player_ptr& player, Vector3 coords, const Vector3& euler = { 0, 0, 0 });
	void teleport_to_player(const player_ptr& player);
	bool teleport_player_to_me(const player_ptr& player);
	void teleport_into_player_vehicle(const player_ptr& player);
	bool teleport_player_to_way_point(const player_ptr& player);
	inline bool force_teleport_when_no_collision_found = false;
	inline bool check_for_interior_collision = false;

	inline bool using_teleport_animation = false;
	inline int teleport_animation = 0;
	inline std::vector<std::string> teleport_animation_names = { "Fade", "Swoop Down", "Smooth" };
}

struct remote_player_teleport
{
	std::int16_t m_player_net_id;
	rage::fvector3 m_position;
};

inline std::unordered_map<uint16_t, remote_player_teleport> g_remote_player_teleports;
