#pragma once
#include "script/home/settings/header/header.h"

namespace image_loader
{
	struct vec2
	{
		int x, y;
	};

	struct img
	{
		unsigned char* data;
		vec2 v2;
	};

	struct img_data
	{
		unsigned char* img_bytes;
		int img_len;
		int frame_delay;
	};

	struct frame_data
	{
		int frame_delay;
		ID3D11ShaderResourceView* res_view;
	};

	ID3D11ShaderResourceView* create_texture(const fs::path& file_path);
	std::map<int, frame_data> create_gif_texture(const fs::path& path);
	ID3D11ShaderResourceView* create_resource_view(unsigned char* img_data, vec2 img_size);

	inline auto m_header_frame = 0;
	inline auto last_frame_count = 0;
	inline std::map<int, frame_data> m_header;

	void flip_through_frames(int delay);
	void header_handler();

	inline bool has_header_loaded = false;

	inline std::vector<std::string> supported_image_formats = {
		".png", ".jpeg", ".bmp", ".psd", ".tga", ".gif", ".hdr", ".pic", ".ppm", ".pgm", ".jpg"
	};

	inline bool contain_pict_ext(const fs::path& path)
	{
		return contains(supported_image_formats, path.extension().string());
	}
}
