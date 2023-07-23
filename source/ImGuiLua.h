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

#ifndef IMGUILUA_H_
#define IMGUILUA_H_

#include "LuaValue.h"

// Class used by games panels to interact with the Lua files responsible for drawing the interface
class ImGuiLua
{
public:
	static bool Init();
	static void Quit();
	static void Reload();

	static void ShowErrors();

	static bool SyncDataStore(const LuaValueMap& dataStore);
	static bool CallDrawFunction(const std::string& functionName);
};

#endif
