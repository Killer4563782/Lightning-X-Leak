#include "give_collectibles.h"
#include "services/online/online.h"
#include "gui/types/function_option.hpp"
#include "services/players/player_manager.hpp"

namespace ui::submenus
{
	void give_collectibles(const eCollectibleType type)
	{
		g_fiber_pool->queue_job([&]
		{
			for (auto i = 0; i < 10; i++)
			{
				services::give_collectible(g_player_manager->get_selected()->id(), type, i);
			}
		});
	}

	void give_collectibles_submenu::on_init()
	{
		this->add(function_option("All", {}, [this]
		{
			give_collectibles(eCollectibleType::MovieProp);
			give_collectibles(eCollectibleType::CacheShard);
			give_collectibles(eCollectibleType::ChestShard);
			give_collectibles(eCollectibleType::RadioTowerShard);
			give_collectibles(eCollectibleType::AudioPlayerShard);
			give_collectibles(eCollectibleType::ShipwreckShard);
			give_collectibles(eCollectibleType::BuriedStashShard);
			give_collectibles(eCollectibleType::Treat);
			give_collectibles(eCollectibleType::Organics);
			give_collectibles(eCollectibleType::Skydive);
		}));
		this->add(function_option("Movie Prop", {}, [this]
		{
			give_collectibles(eCollectibleType::MovieProp);
		}));
		this->add(function_option("Cache Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::CacheShard);
		}));
		this->add(function_option("Chest Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::ChestShard);
		}));
		this->add(function_option("Radio Tower Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::RadioTowerShard);
		}));
		this->add(function_option("Audio Player Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::AudioPlayerShard);
		}));
		this->add(function_option("Ship wreck Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::ShipwreckShard);
		}));
		this->add(function_option("Buried Stash Shard", {}, [this]
		{
			give_collectibles(eCollectibleType::BuriedStashShard);
		}));
		this->add(function_option("Treat", {}, [this]
		{
			give_collectibles(eCollectibleType::Treat);
		}));
		this->add(function_option("Organics", {}, [this]
		{
			give_collectibles(eCollectibleType::Organics);
		}));
		this->add(function_option("Skydive", {}, [this]
		{
			give_collectibles(eCollectibleType::Skydive);
		}));
	}
}
