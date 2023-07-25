/* ImGuiLua.h
Copyright (c) 2014 by Michael Zahniser

Endless Sky is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

Endless Sky is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef IMGUI_LUA_BINDINGS_H_
#define IMGUI_LUA_BINDINGS_H_

#include <functional>

// Avoid including whole ImGui in header
typedef uint32_t ImTextureID;

struct lua_State;
const char* CallFunction(const char* functionName);
void LoadImguiBindings(lua_State* L, std::function<ImTextureID (const char*)> converter);
void UnrollImguiStack();
#endif
