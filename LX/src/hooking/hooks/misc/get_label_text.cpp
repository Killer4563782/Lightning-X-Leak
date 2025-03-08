#include "hooking/hooking.hpp"
#include "services/custom_text/custom_text_service.hpp"

const char* hooks::get_label_text(void* unk, const char* text_code)
{
	if (const auto text = g_custom_text_service->get_text(text_code); text)
		return text;

	return g_hooking->get_original<get_label_text>()(unk, text_code);
}
