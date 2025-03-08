#include "custom_header.h"

#include "fonts/icons.h"
#include "gui/types/break_option.hpp"
#include "gui/types/function_option.hpp"
#include "gui/types/icon_option.hpp"
#include "helpers/imageLoaderHelpers.h"
#include "thread/threadpool.h"

namespace ui::submenus
{
	void custom_header_submenu::on_enter()
	{
		m_options.clear();
		const auto headers_path = g_lx_path / "Headers";

		if (!exists(headers_path))
			create_directory(headers_path);

		this->add(icon_option("Open Headers Folder", "Access the headers folder", ICON_FA_FOLDER, [headers_path]
		{
			ShellExecuteA(nullptr, "open", headers_path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		}));

		if (!is_empty(headers_path))
		{
			this->add(break_option("Custom Headers"));
			for (const auto& dir_entry : fs::directory_iterator(headers_path))
			{
				if (!dir_entry.is_regular_file() || !image_loader::contain_pict_ext(dir_entry.path()))
				{
					this->add(option(
						std::format("Invalid: {}", dir_entry.path().filename().string()),
						std::format("Make sure file contains any of the following extensions: {}", join(image_loader::supported_image_formats, ", "))
					));
					continue;
				}

				this->add(function_option(dir_entry.path().filename().string(), {}, [path = dir_entry.path(), this]
				{
					if (!exists(path))
					{
						notify(Info, "The header does not exist anymore");
						return;
					}

					if (texture_loading_in_progress)
					{
						notify(Info, "Custom header loading is already in progress. Please wait");
						return;
					}

					texture_loading_in_progress = true;
					g_thread_pool->push([path, this]
					{
						image_loader::has_header_loaded = false;
						image_loader::m_header.clear();
						image_loader::m_header_frame = 0;

						if (path.extension() == ".gif")
						{
							image_loader::m_header = image_loader::create_gif_texture(path);
						}
						else
						{
							image_loader::m_header.try_emplace(0, 0, image_loader::create_texture(path));
						}

						image_loader::has_header_loaded = true;
						texture_loading_in_progress = false;
					});
				}
				));
			}
		}
	}
}
