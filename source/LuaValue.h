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

typedef std::map<std::string, class LuaValue> LuaValueMap;

// A Quick'n'drity value class for storing the different type we want to send to Lua
// Would much rather have used std::variant :(
class LuaValue
{
public:
	enum class ValueType { Nil, Bool, Int, Double, String, ValueMap };

	LuaValue(bool value);
	LuaValue(int64_t value);
	LuaValue(double value);
	LuaValue(const std::string& value);
	LuaValue(LuaValueMap&& value);

	LuaValue(LuaValue&& other);

	~LuaValue();

	// Movable but not copyable or we'd have to deep-copy value maps
	LuaValue(const LuaValue&) = delete;
	LuaValue& operator=(const LuaValue&) = delete;

	void set(bool value);
	void set(int64_t value);
	void set(double value);
	void set(const std::string& value);
	LuaValueMap& set(LuaValueMap&& value);

	ValueType getType();

	bool isNil() const;
	bool getBool() const;
	int64_t getInt() const;
	double getDouble() const;
	const std::string& getString() const;
	const LuaValueMap& getMap() const;

private:
	void release();

private:
	ValueType m_type{ ValueType::Nil };
	union {
		bool valueBool;
		int64_t valueInt;
		double valueDouble;
		std::string* valueString;
		LuaValueMap* valueMap;
	} m_value;
};

#endif

