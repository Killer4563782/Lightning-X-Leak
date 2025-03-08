#include "hooking/hooking.hpp"
#include "commands/manager.hpp"
#include <misc/vfx/TimecycleKeyframeData.hpp>
#include <commands/settings.h>
#include <gui/types/toggle_option.hpp>

inline rage::fvector4 color_to_vector(const ui::color& color)
{
    return rage::fvector4(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f);
}

void hooks::update_timecycle_keyframe_data(int64_t timecycleManager, TimecycleKeyframeData* timecycleKeyframeData)
{
	g_hooking->get_original<update_timecycle_keyframe_data>()(timecycleManager, timecycleKeyframeData);
    if (commands::settings::g_vfx.enable_custom_sky_color) 
    {
        if ("rainbow_sky"_LC->m_toggle)
        {
            //TO FIX
            /*timecycleKeyframeData->m_azimuth_east = rgb_to_vector(commands::settings::sky_rgb);
            timecycleKeyframeData->m_azimuth_west = rgb_to_vector(commands::settings::sky_rgb);
            timecycleKeyframeData->m_azimuth_transition = rgb_to_vector(commands::settings::sky_rgb);
            timecycleKeyframeData->m_zenith = rgb_to_vector(commands::settings::sky_rgb);*/
        }
        else
        {
            timecycleKeyframeData->m_azimuth_east = color_to_vector(commands::settings::g_vfx.azimuth_east);
            timecycleKeyframeData->m_azimuth_west = color_to_vector(commands::settings::g_vfx.azimuth_west);
            timecycleKeyframeData->m_azimuth_transition = color_to_vector(commands::settings::g_vfx.azimuth_transition);
            timecycleKeyframeData->m_zenith = color_to_vector(commands::settings::g_vfx.zenith);
        }
    }
}
