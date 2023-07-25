#include "ImGuiLua.h"
#include "ImGuiLua.h"

#include "Files.h"
#include "Logger.h"
#include "LuaValue.h"
#include "Sprite.h"
#include "SpriteSet.h"

#include "imgui/imgui.h"
#include "imgui/lua_bindings/imgui_lua_bindings.h"
#include "lua.hpp"

#include <math.h>

using namespace std;

namespace {
	// Static members
	constexpr const char* RELATIVE_PATH{ "_ui/panels.lua" };
	constexpr const char* DATA_TABLE_NAME = "data";
	constexpr const char* ON_UI_ACTION_NAME = "SendEvent";

	lua_State* luaState{ nullptr }; // Current lua_state for all UI
	vector<string> errors; // Error stack for showing in popup window

	LuaValueMap pendingData; // Data that needs to be synced with the UI Lua script next frame
	LuaValueMap backupData; // This holds all values in case we need to refresh them, e.g. we reload the UI script

	string drawFunctionName; // The last executed draw function name, used for error messages 
	OnUIActionHandler onActionHandler; // Function to call when the UI script sends and action event

	// ------------------------------------------------------------------------

	// Helper for error handling
	void ReportError(std::string error)
	{
		errors.push_back(error);
		Logger::LogError("ImGuiLua: "+ errors.back());
	}



	// Converter from file to texture_id so that Lua scripts can use file names
	ImTextureID getTextureForImageName(const char* imageFile)
	{
		const Sprite* sprite = SpriteSet::Get(imageFile);
		return sprite ? sprite->Texture() : uint32_t(0);
	}



	// Sync a given value map to the Lua data table
	void SyncTable(const LuaValueMap& data)
	{
		// This should get the data table and push it onto the stack
		lua_getglobal(luaState, DATA_TABLE_NAME);

		// Sync new data with Lua table
		for (const auto& pair : data)
		{
			pair.second.PushValueToLua(luaState);
			/* Set stack value as field on table indicated by the -2 index (2 from stack top) */
			lua_setfield(luaState, -2, pair.first.c_str());
		}

		// Pop table
		lua_pop(luaState, 1);
	}



	// Function called by the UI script when it wants to trigger a change in the C++ code,
	// for instance when the player clicks a button
	int OnUIAction(lua_State* L)
	{
		if (!onActionHandler)
		{
			ReportError("No UI action handler registered for draw function '" + drawFunctionName + "'.");
			return 0;
		}

		const int maxArgs = lua_gettop(L);
		int arg = 1;

		// First argument a string id for the action, e.g. "MainMenu.LoadGame"
		const char* actionId = maxArgs > 0 ? lua_tostring(L, arg++) : nullptr;
		if (!actionId)
		{
			ReportError("Missing action id. Function: " + drawFunctionName);
			return 0;
		}

		// We accept an optional Lua table so the script can pack in any named data
		LuaValueMap table;
		if (maxArgs >= arg)
		{
			if (!lua_istable(L, arg))
			{
				ReportError("Second parameter must be a Table. Action Id: " + string(ON_UI_ACTION_NAME) + ", Function: " + drawFunctionName);
				return 0;
			}

			// Loop through table and convert to a map
			lua_pushnil(L);  // First key
			while (lua_next(L, arg) != 0)
			{
				LuaValueMap::value_type entry( lua_tostring(L, -2), LuaValue());
				if (!entry.second.ReadValueFromLua(L, -1))
				{
					ReportError("Argument table value '" + entry.first + "' is an unhandled type. Action Id: " + string(ON_UI_ACTION_NAME) + ", Function: " + drawFunctionName);
				}
				table.insert(std::move(entry));
				// removes 'value'; keeps 'key' for next iteration
				lua_pop(L, 1);
			}
			++arg;
		}
		onActionHandler(actionId, table);
		return 0;
	}
}



bool ImGuiLua::Init()
{
	luaState = luaL_newstate();

	luaL_openlibs(luaState);

	std::string file = Files::Data() + RELATIVE_PATH;
	if (luaL_dofile(luaState, file.c_str())) {
		string luaError = lua_tostring(luaState, -1);
		ReportError("Failed to load UI Lua file at '" + file + "'\n\n Lua Error: \n" + luaError);
		lua_pop(luaState, 1);
		return false;
	}

	LoadImguiBindings(luaState, getTextureForImageName);

	// Create an empty table and set it on the script for filling later
	lua_newtable(luaState);
	lua_setglobal(luaState, DATA_TABLE_NAME);

	// Set up a single UI interaction function
	// The idea is to to provide a flexible and robust system for sending messages
	// to the C++ UI code without relying on panels adding their own function table
	lua_pushcfunction(luaState, &OnUIAction);
	lua_setglobal(luaState, ON_UI_ACTION_NAME);

	return true;
}



void ImGuiLua::Quit()
{
	if (luaState)
	{
		lua_close(luaState);
		luaState = nullptr;
	}
}



void ImGuiLua::Reload()
{
	Quit();
	Init();
	SyncTable(backupData);
	SyncPendingData();
}



void ImGuiLua::SetValueToNil(const std::string& key)
{
	if (backupData[key] != LuaValue())
	{
		pendingData.emplace(key, LuaValue());
	}
}



void ImGuiLua::SetValue(const std::string& key, bool value)
{
	if (backupData[key] != value)
	{
		pendingData.emplace(key, value);
	}
}



void ImGuiLua::SetValue(const std::string& key, int64_t value)
{
	if (backupData[key] != value)
	{
		pendingData.emplace(key, value);
	}
}



void ImGuiLua::SetValue(const std::string& key, double value)
{
	if (backupData[key] != value)
	{
		pendingData.emplace(key, value);
	}
}



void ImGuiLua::SetValue(const std::string& key, const std::string& value)
{
	if (backupData[key] != value)
	{
		pendingData.emplace(key, value);
	}
}



void ImGuiLua::SyncPendingData()
{
	// Before syncing we update some global state information
	ImGuiIO& io = ImGui::GetIO();
	SetValue("system.displayWidth", io.DisplaySize.x);
	SetValue("system.displayHeight", io.DisplaySize.y);

	if (pendingData.empty())
		return;

	LuaValueMap& pending = pendingData;
	LuaValueMap& backup = backupData;

	// Send data to Lua
	SyncTable(pending);

	// Store pending data on backup in case of reload
	for (const auto& entry : pending)
	{
		// I've tried inserting the pairs from pending straight into backup but for some reason the pair copy skips the LuaValue's copy constructor/operator
		// I guess I'm doing something wrong but no idea what so I give up for now...
		backup[entry.first] = entry.second;
	}
	pending.clear();

	// C++17
	//backupData.merge(pendingData);
}



bool ImGuiLua::CallDrawFunction(const string& functionName, OnUIActionHandler _onActionHandler)
{
	if (errors.size() > 0)
		return false;

	drawFunctionName = functionName;
	onActionHandler = _onActionHandler;

	const char* luaError = CallFunction(functionName.c_str());
	if (luaError)
	{
		ReportError(luaError);
		return false;
	}
	return true;
}



void ImGuiLua::DebugUIDataWindow(bool* opened)
{
	ImGui::Begin("UI Data Viewer", opened);
	ImGui::BeginListBox("UI Data List", { -FLT_MIN, -FLT_MIN });
	for (const auto& pair : backupData)
	{
		ImGui::LabelText(pair.second.ToString().c_str(), "%s", pair.first.c_str());
	}
	ImGui::EndListBox();
	ImGui::End();
}



void ImGuiLua::DebugShowErrors()
{
	if (errors.size())
	{
		string text = errors.front();
		for (int i = 1; i < errors.size(); ++i)
		{
			text += '\n' + errors[i];
		}

		ImGui::Begin("ErrorWindow", nullptr, ImGuiWindowFlags_NoDecoration);
		ImGui::TextUnformatted(text.c_str());
		if (ImGui::Button("OK"))
		{
			errors = {};
		}
		ImGui::End();
	}
}
