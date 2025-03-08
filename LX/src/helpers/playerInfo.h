#pragma once
#include "curl/curl.h"
#include "gui/data.h"
#include "services/players/player_manager.hpp"

namespace player_info
{
	struct scene_preset
	{
		struct scene_preset_element
		{
			char _0x0000[24];
			rage::vector4<float> m_position;
			rage::vector4<float> m_position_43;
			rage::vector4<float> m_rotation_xyz;
			rage::vector4<float> m_bg_rect_xywh;
			rage::vector4<float> m_bg_rect_xywh_43;
			uint32_t m_bg_rect_color;
			uint32_t m_blend_color;
			bool m_enabled;
			char _0x0071[15];
		};

		uint32_t m_name;
		char _0x0004[4];
		scene_preset_element m_elements[5];
		char _0x0288[8];
		int32_t m_element_count;
		char _0x0294[12];

		void set_element_rot(uint32_t element, rage::vector3<float> rot)
		{
			if (element < 5)
			{
				m_elements[element].m_rotation_xyz.x = rot.x;
				m_elements[element].m_rotation_xyz.y = rot.y;
				m_elements[element].m_rotation_xyz.z = rot.z;
			}
		}

		void set_element_pos(uint32_t element, rage::vector2<float> xy, rage::vector2<float> wh)
		{
			if (element < 5)
			{
				m_elements[element].m_bg_rect_xywh.z = wh.x; // make fullscreen
				m_elements[element].m_bg_rect_xywh.w = wh.y; // make fullscreen
				m_elements[element].m_bg_rect_xywh.x = xy.x; // convert and set pos
				m_elements[element].m_bg_rect_xywh.y = xy.y; // convert and set pos
				m_elements[element].m_bg_rect_xywh_43.z = wh.x; // make fullscreen
				m_elements[element].m_bg_rect_xywh_43.w = wh.y; // make fullscreen
				m_elements[element].m_bg_rect_xywh_43.x = xy.x; // convert and set pos
				m_elements[element].m_bg_rect_xywh_43.y = xy.y; // convert and set pos
				m_elements[element].m_bg_rect_color = 0;
			}
		}
	};

	static scene_preset* get_scene_preset(int64_t base_address, const uint32_t* scene_id)
	{
		unsigned index = 0;
		int64_t preset_count = 0;
		const int64_t preset_array_base = *reinterpret_cast<uint16_t*>(base_address + 0x810);

		if (!preset_array_base) 
			return nullptr;

		const int64_t scene_preset_array = *reinterpret_cast<uint64_t*>(base_address + 0x808);
		const uint32_t target_scene_id = *scene_id;
		for (auto current_id = reinterpret_cast<uint32_t*>(scene_preset_array); *current_id != target_scene_id; current_id += 0xA8)
		{
			++preset_count;
			++index;
			if (preset_count >= preset_array_base) 
				return nullptr;
		}
		return reinterpret_cast<scene_preset*>(scene_preset_array + 0x2A0 * static_cast<long long>(index));
	}

	inline std::string vehicle_label{};
	inline std::string player_vehicle_god{};
	inline ui::color g_background_color = {10, 10, 10, 200};

	std::string get_player_tags(const player_ptr& player);

	void draw_title(const std::string& title, const ImVec2& pos, const ImVec2& size);
	void draw_single_line(const std::string& icon, const std::string& text, const std::string& data, const ImVec2& pos, const ImVec2& size);
	void draw_single_line_number(const int& number, const std::string& text, const std::string& data, const ImVec2& pos, const ImVec2& size);
	void draw_double_line(const std::string& icon1, const std::string& text1, const std::string& data1, const std::string& icon2, const std::string& text2, const std::string& data2, const ImVec2& pos, const ImVec2& size);

	enum geo_location_status
	{
		Success,
		NotReady,
		Failed
	};

	struct geo_location
	{
		geo_location_status status;
		std::string country = "N/A";
		std::string region = "N/A";
		std::string city = "N/A";
		std::string postcode = "N/A";
		std::string provider = "N/A";
		bool isVPN = false;
		std::string vpnName = "N/A";
	};
	
	geo_location_status get_geo_location(const std::string& ip, geo_location* out);
	inline std::map<std::string, geo_location> geo_location_cache;
	size_t write_callback(const void* contents, size_t size, size_t nmemb, std::string* buffer);
	void draw_on_screen_ped(CPed* ped, rage::vector2<float> pos, rage::vector2<float> scale, rage::vector3<float> position_offset);
	void player_info(const player_ptr& player); // Draws the player info
	void friend_info(const FriendInfo* friends); // Draws the friend info
	void session_host_queue_info(); // Draws the host queue info
}
