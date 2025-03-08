#pragma once
#include "option.hpp"

namespace ui
{
	class submenu;

	namespace menu
	{
		void push(submenu* sub);
		void pop();
	}
}

namespace ui
{
	class submenu : public option
	{
	public:
		submenu* m_parent;
		std::vector<option*> m_options;
		uint64_t m_current = 0;
		uint64_t m_scroll_position = 0;
		bool m_initialized = false;

		explicit submenu(std::string name, std::string description = {}, submenu* parent = nullptr) : option(std::move(name), std::move(description), OptionType::Submenu), m_parent(parent)
		{
		}

		explicit submenu() = default;

		void draw(bool selected) override
		{
			option::draw(selected);

			g_base -= g_options.m_size;

			drawing::text(
				g_options.m_toggle_font,
				"e",
				{
					g_pos.x + g_width + 0.003f,
					g_base + g_options.m_size / 2.f - drawing::get_text_height(
						g_options.m_toggle_font, g_options.m_text_size) / 2.f
				},
				g_options.m_text_size,
				g_options.text_color(selected),
				eJustify::Right,
				-g_options.m_padding,
				g_width
			);

			g_base += g_options.m_size;
		}

		virtual void on_init() { m_initialized = true; }

		virtual void on_tick()
		{
		} // Called once every draw. Useful for changing options
		virtual void on_enter()
		{
		} // Called when entering the menu. Useful for when having to update the options but only once before entering

		template <typename T>
		void add(T* opt)
		{
			static_assert(std::is_base_of_v<option, T>, "T must derive from option");

			if (opt->m_type == OptionType::Submenu)
				if (auto submenu_opt = dynamic_cast<submenu*>(opt); submenu_opt && !submenu_opt->m_parent)
					submenu_opt->m_parent = this;

			m_options.emplace_back(opt);
		}

		template <typename T>
		void add(T opt)
		{
			static_assert(std::is_base_of_v<option, T>, "T must derive from option");

			auto new_opt = std::make_unique<T>(std::move(opt));
			add(new_opt.get());
			new_opt.release();
		}

		void clear()
		{
			m_options.clear();
		}

		option* get_option(uint64_t index) const
		{
			return index < m_options.size() ? m_options[index] : nullptr;
		}

		option* get_option() const
		{
			return get_option(m_current);
		}

		uint64_t calculate_new_index(ActionType type, uint64_t index, int attempt = 0) {
		    if (attempt > 1 || m_options.size() <= 1) return 0;
		
		    uint64_t offset = (attempt == 1) ? 0 : 1;
		
		    if (type == ActionType::Down) {
		        while (index + offset < m_options.size() && m_options[index + offset]->m_type == OptionType::Break) {
		            ++offset;
		        }
		
		        if (index + offset >= m_options.size() || m_options[index + offset]->m_type == OptionType::Break) {
		            return calculate_new_index(ActionType::Down, 0, attempt + 1);
		        }
		
		        if (attempt == 1) {
		            m_scroll_position = (index >= g_options.m_max_options_visible_per_submenu)
		                ? index - g_options.m_max_options_visible_per_submenu
		                : 0;
		        }
		
		        return index + offset;
		    }
		
		    if (type == ActionType::Up) {
		        while (index >= offset && m_options[index - offset]->m_type == OptionType::Break) {
		            ++offset;
		        }
		
		        if (offset > index || m_options[index - offset]->m_type == OptionType::Break) {
		            return calculate_new_index(ActionType::Up, m_options.size() - 1, attempt + 1);
		        }
		
		        if (attempt == 1) {
		            if (m_options.size() >= g_options.m_max_options_visible_per_submenu && 
		                index >= m_options.size() - g_options.m_max_options_visible_per_submenu) {
		                m_scroll_position = m_options.size() - g_options.m_max_options_visible_per_submenu;
		            } else {
		                m_scroll_position = (index >= g_options.m_max_options_visible_per_submenu) 
		                    ? index - g_options.m_max_options_visible_per_submenu 
		                    : 0;
		            }
		        }
		
		        return index - offset;
		    }
		
		    return 0;
		}
		
		public:
		void action(ActionType type, bool first_receiver = true) override {
		    if (first_receiver) {
		        if (auto opt = get_option()) {
		            opt->action(type, false);
		        }
		
		        if (!m_options.empty()) {
		            switch (type) {
		                case ActionType::Up:
		                case ActionType::Down: {
		                    m_current = std::clamp(calculate_new_index(type, m_current), 0ULL, m_options.size() - 1);
		
		                    // Adjust scroll position dynamically
		                    size_t max_visible = g_options.m_max_options_visible_per_submenu;
		
		                    if (m_current < m_scroll_position + 2) {
		                        m_scroll_position = (m_current > 2) ? m_current - 2 : 0;
		                    } 
		                    else if (m_current > m_scroll_position + max_visible - 4) {
		                        m_scroll_position = std::min(m_current - (max_visible - 3), 
		                            m_options.size() > max_visible ? m_options.size() - max_visible : 0);
		                    }
		                    break;
		                }
		            }
		        }
		    } 
		    else if (type == ActionType::Enter) {
		        if (!m_initialized) {
		            on_init();
		            m_initialized = true;
		        }
		
		        if (auto opt = get_option(); opt && opt->m_type == OptionType::Break) {
		            action(ActionType::Down, true);
		        }
		
		        on_enter();
		        menu::push(this);
		    }
		}
	};
}
