#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

// Windows Includes
#include <Windows.h>
#include <windef.h>
#include <winnt.h>
#include <winternl.h>
#include <Psapi.h>

// C++ Includes
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <d3d11.h>
#include <d3d9.h>
#include <DbgHelp.h>
#include <dxgi.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <ostream>
#include <random>
#include <ranges>
#include <regex>
#include <set>
#include <shellapi.h>
#include <ShlObj.h>
#include <shlobj_core.h>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <string_view>
#include <timeapi.h>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <xstring>

// Menu Includes
#include "core/logger.h"
#include "functions/strings.h"
#include "imgui/imgui.h"
#include "rage/enums.h"
#include "rage/joaat.hpp"
#include "rage/commands/list.h"
#include "services/notification/notification.h"

// ImGui WndProc
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Namespaces
using namespace std::chrono;
using namespace std::chrono_literals;
namespace fs = std::filesystem;