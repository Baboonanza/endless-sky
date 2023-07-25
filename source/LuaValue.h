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

#ifndef LUAVALUE_H_
#define LUAVALUE_H_

#include <map>
#include <stdint.h>
#include <string>

struct lua_State;

// Map to emulate Lua's table
typedef std::map<std::string, class LuaValue> LuaValueMap;

// A Quick'n'dirty value class for storing the different type we want to
// send to Lua. A poor man's std::variant :(
class LuaValue
{
public:
	enum class ValueType { Nil, Bool, Integer, Number, String };

	LuaValue() = default;
	LuaValue(bool value);
	LuaValue(int64_t value);
	LuaValue(double value);
	LuaValue(std::string value);
	LuaValue(const LuaValue& other);

	LuaValue& operator=(const LuaValue& other);
	bool operator==(const LuaValue& other) const;
	bool operator!=(const LuaValue& other) const;

	std::string ToString() const;

	// Lua interactions, not for direct panel use
	bool ReadValueFromLua(lua_State* L, int stackIx);
	void PushValueToLua(lua_State* L) const;

private:
	ValueType m_type{ ValueType::Nil };
	union {
		bool valueBool;
		int64_t valueInteger;
		double valueNumber;
	} m_value;
	// Keeping this outside the union is unpleasant but saves so much hassle with heap allocation, move operators etc.
	std::string m_string; 
};

#endif
