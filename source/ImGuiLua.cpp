#include "ImGuiLua.h"

#include "Files.h"
#include "Logger.h"
#include "Sprite.h"
#include "SpriteSet.h"

#include "imgui/imgui.h"
#include "imgui/lua_bindings/imgui_lua_bindings.h"
#include "lua.hpp"

#include <string>
#include <stdexcept>

using namespace std;

constexpr const char* relativePath{"_ui/panels.lua"};



namespace {
	lua_State* luaState{ nullptr };
	vector<string> errors;

	ImTextureID getTextureForImageName(const char* imageFile)
	{
		const Sprite* sprite = SpriteSet::Get(imageFile);
		return sprite ? sprite->Texture() : uint32_t(0);
	}
}



bool ImGuiLua::Init()
{
	luaState = luaL_newstate();

	luaL_openlibs(luaState);

	std::string file = Files::Data() + relativePath;
	if (luaL_dofile(luaState, file.c_str())) {
		string luaError = lua_tostring(luaState, -1);
		errors.push_back("Failed to load UI Lua file at '" + file + "'\n\n Lua Error: \n" + luaError);
		Logger::LogError(errors.back());
		lua_pop(luaState, 1);
		return false;
	}

	LoadImguiBindings(luaState, getTextureForImageName);

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
}

void ImGuiLua::ShowErrors()
{
	if ( errors.size() )
	{
		ImGui::Begin("ErrorWindow", nullptr, ImGuiWindowFlags_NoDecoration);
		if (ImGui::Button("OK"))
		{
			errors = {};
		}
		ImGui::End();
	}
}

bool ImGuiLua::SyncDataStore(const LuaValueMap& dataStore)
{
	return false;
}

bool ImGuiLua::CallDrawFunction(const string& functionName)
{
	lua_getglobal(luaState, functionName.c_str());
	if (lua_pcall(luaState, 0, 0, 0) != 0)
	{
		string luaError = lua_tostring(luaState, -1);
		errors.push_back("Error running function '" + functionName + "'\n\n Lua Error: \n" + luaError);
	}
	return false;
}
