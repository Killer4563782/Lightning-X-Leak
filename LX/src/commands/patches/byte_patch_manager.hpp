#pragma once

extern "C" uint64_t g_sound_overload_ret_addr;
inline std::mutex g_patches_mutex;

class byte_patch_manager
{
public:
	byte_patch_manager();
	~byte_patch_manager();
};

inline std::unique_ptr<byte_patch_manager> g_byte_patch_manager{};
