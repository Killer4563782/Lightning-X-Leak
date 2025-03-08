#include "appearance.h"
#include "animations.h"
#include "commands/manager.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/vector_bool_option.hpp"
#include "outfit editor/outfit_editor.h"
#include "model_changer_submenu.h"
#include "walk_style_submenu.h"

namespace ui::submenus
{
	void appearance_submenu::on_init()
	{
		this->add(model_changer_submenu("Model Changer", {}));
		this->add(outfit_editor_submenu("Outfit Editor", {})); 
		this->add(animations_submenu("Animations", {}));
		this->add(walk_style_submenu("Walk Style", {}));
		this->add(vector_bool_option("invisible"_LVC));
		this->add(function_option("clean_ped"_C));
	}
}
