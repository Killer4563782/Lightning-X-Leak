#pragma once
#include "atArray.hpp"

namespace rage
{
	using atHashString = u32;

	class gameSkeleton
	{
	public:
		using fnInitFunction = void(*)(u32);
		using fnShutdownFunction = void(*)(u32);
		using fnUpdateFunction = void(*)();

		virtual ~gameSkeleton()
		{
		}

		void SetCurrentInitType(u32 initType) { m_CurrentInitType = initType; }
		void SetCurrentShutdownType(u32 shutdownType) { m_CurrentShutdownType = shutdownType; }
		void SetCurrentDependencyLevel(u32 dependencyLevel) { m_CurrentDependencyLevel = dependencyLevel; }
		virtual void Init(u32 initMode) = 0;
		virtual void Shutdown(u32 shutdownMode) = 0;
		virtual void Update(u32 updateMode) = 0;
		static constexpr u32 DEFAULT_MAX_SYSTEMS_REGISTERED = 128;
		static constexpr u32 MAX_DEPENDENCY_LEVELS = 32;
		static constexpr u32 MAX_NESTED_UPDATE_GROUPS = 32;

		struct systemData
		{
			systemData()
			{
			}

			bool InitialisesThisMode(u32 initMode) const { return ((m_InitTypes & initMode) != 0); }
			bool ShutdownsThisMode(u32 shutdownMode) const { return ((m_ShutdownTypes & shutdownMode) != 0); }
			fnInitFunction m_InitFunction;
			fnShutdownFunction m_ShutdownFunction;
			u32 m_InitDependencyLevel;
			u32 m_ShutdownDependencyLevel;
			u32 m_InitTypes;
			u32 m_ShutdownTypes;
			atHashString m_SystemName;
		};

		struct dependency
		{
			dependency(u32 level) :
				dependencyLevel(level)
				, next(nullptr)
			{
			}

			u32 dependencyLevel;
			atArray<u32> sysData;
			dependency* next;
		};

		struct mode
		{
			mode(u32 type) :
				modeType(type)
				, head(nullptr)
				, next(nullptr)
			{
			}

			u32 modeType;
			dependency* head;
			mode* next;
		};

		struct updateBase
		{
			updateBase(const atHashString& name,
			           bool addTimebar,
			           float timebarBudget) :
				m_AddTimebar(addTimebar)
				, m_TimebarBudget(timebarBudget)
				, m_Name(name)
				, m_Next(nullptr)
			{
			}

			virtual ~updateBase()
			{
			}

			virtual void Update() = 0;

			bool m_AddTimebar;
			float m_TimebarBudget;
			atHashString m_Name;
			updateBase* m_Next;
		};

		struct updateElement : updateBase
		{
			updateElement() :
				updateBase(atHashString(), false, 0.0f)
			{
			}

			updateElement(const atHashString& name,
			              bool addTimebar,
			              float timebarBudget,
			              fnUpdateFunction updateFunction) :
				updateBase(name, addTimebar, timebarBudget)
				, m_UpdateFunction(updateFunction)
			{
			}

			fnUpdateFunction m_UpdateFunction;
		};

		struct updateGroup : updateBase
		{
			updateGroup(const atHashString& name,
			            bool addTimebar,
			            float timebarBudget) :
				updateBase(name, addTimebar, timebarBudget)
				, m_Head(nullptr)
			{
			}

			~updateGroup() override
			{
				updateBase* element = m_Head;
				while (element)
				{
					updateBase* next = element->m_Next;
					delete element;
					element = next;
				}
			}

			updateBase* GetElement(u32 name)
			{
				updateBase* element = m_Head;
				while (element)
				{
					if (element->m_Name == name)
					{
						return element;
					}
					element = element->m_Next;
				}
				return nullptr;
			}

			updateBase* m_Head;
		};

		struct updateMode
		{
			updateMode(u32 type) :
				m_ModeType(type)
				, m_Head(nullptr)
				, m_Next(nullptr)
			{
			}

			~updateMode()
			{
				updateBase* element = m_Head;
				while (element)
				{
					updateBase* next = element->m_Next;
					delete element;
					element = next;
				}
			}

			void Update()
			{
				updateBase* element = m_Head;
				while (element)
				{
					element->Update();
					element = element->m_Next;
				}
			}

			updateBase* GetElement(u32 name)
			{
				updateBase* element = m_Head;
				while (element)
				{
					if (element->m_Name == name)
					{
						return element;
					}
					element = element->m_Next;
				}
				return nullptr;
			}

			u32 m_ModeType;
			updateBase* m_Head;
			updateMode* m_Next;
		};

		const mode* GetMode(const mode* modeRoot, u32 modeType) const
		{
			const mode* currMode = modeRoot;

			while (currMode && currMode->modeType != modeType)
			{
				currMode = currMode->next;
			}

			return currMode;
		}

		systemData* GetSystemDataByName(const atHashString& name, unsigned& index)
		{
			int numSystems = m_RegisteredSystemData.count();
			systemData* data{};
			if (numSystems > 0)
			{
				for (int systemIndex = 0; systemIndex < numSystems && !data; systemIndex++)
				{
					systemData& sysData = m_RegisteredSystemData[systemIndex];
					if (name == sysData.m_SystemName)
					{
						data = &sysData;
						index = systemIndex;
					}
				}
			}
			return data;
		}

		u32 m_CurrentDependencyLevel; // The current dependency level, used when building an init/shutdown mode
		u32 m_CurrentInitType; // The current init mode type, used when building an init mode
		u32 m_CurrentShutdownType; // The current shutdown mode type, used when building an shutdown mode
		u32 m_CurrentUpdateType; // The current update mode type, used when building an update mode
		atArray<systemData> m_RegisteredSystemData; // Array of system data, used as storage for data that can be shared between init/shutdown modes
		u32 m_UpdateGroupStackPointer; // The update group stack pointer, used when building an update mode
		updateGroup* m_UpdateGroupStack[MAX_NESTED_UPDATE_GROUPS]; // The update group stack, used when building an update mode
		mode* m_InitModes; // Linked list of different registered initialisation modes
		mode* m_ShutdownModes; // Linked list of different registered shutdown modes
		updateMode* m_UpdateModes; // Linked list of different registered update modes
	};

	static_assert(sizeof(gameSkeleton) == 0x148);
}