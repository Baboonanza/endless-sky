#include "LuaValue.h"
#include "LuaValue.h"
#include "LuaValue.h"

#include "lua.hpp"

using namespace std;

constexpr double NUMBER_EQUALITY_TOLERANCE{ 0.0001 };

LuaValue::LuaValue(bool value)
	: m_type{ ValueType::Bool }
{
	m_value.valueBool = value;
}



LuaValue::LuaValue(int64_t value)
	: m_type{ ValueType::Integer }
{
	m_value.valueInteger = value; // maybe need to use lua_numbertointeger?
} 



LuaValue::LuaValue(double value)
	: m_type{ ValueType::Number }
{
	m_value.valueNumber = value;
}



LuaValue::LuaValue(string value)
	: m_type{ ValueType::String }, m_string{ value }
{
}



LuaValue::LuaValue(const LuaValue& other)
{
	*this = other;
}



bool LuaValue::ReadValueFromLua(lua_State* L, int stackIx)
{
	switch (lua_type(L, stackIx))
	{
	case LUA_TNIL:
		m_type = ValueType::Nil;
		return true;
	case LUA_TBOOLEAN:
		m_type = ValueType::Bool;
		m_value.valueBool = lua_toboolean(L, stackIx);
		return true;
	case LUA_TNUMBER:
		if (lua_isinteger(L, stackIx))
		{
			m_type = ValueType::Integer;
			m_value.valueInteger = lua_tointeger(L, stackIx);
		}
		else
		{
			m_type = ValueType::Number;
			m_value.valueNumber = lua_tonumber(L, stackIx);
		}
		return true;
	case LUA_TSTRING:
		m_type = ValueType::String;
		m_string = lua_tostring(L, stackIx);
		return true;
	}
	return false;
}



void LuaValue::PushValueToLua(lua_State *L) const
{
	switch (m_type)
	{
	case ValueType::Nil:
		lua_pushnil(L);
		break;
	case ValueType::Bool:
		lua_pushboolean(L, m_value.valueBool);
		break;
	case ValueType::Integer:
		lua_pushinteger(L, static_cast<lua_Integer>(m_value.valueInteger));
		break;
	case ValueType::Number:
		lua_pushnumber(L, static_cast<lua_Number>(m_value.valueNumber));
		break;
	case ValueType::String:
		lua_pushstring(L, m_string.c_str());
		break;
	}
}



LuaValue& LuaValue::operator=(const LuaValue& other)
{
	m_type = other.m_type;
	switch (m_type)
	{
	case ValueType::Nil:
		break;
	case ValueType::Bool:
		m_value.valueBool = other.m_value.valueBool;
		break;
	case ValueType::Integer:
		m_value.valueInteger = other.m_value.valueInteger;
		break;
	case ValueType::Number:
		m_value.valueNumber = other.m_value.valueNumber;
		break;
	case ValueType::String:
		m_string = other.m_string;
		break;
	}
	return *this;
}



bool LuaValue::operator==(const LuaValue& other) const
{
	if (m_type != other.m_type)
		return false;
	switch (m_type)
	{
	case ValueType::Nil:
		return true;
	case ValueType::Bool: 
		return m_value.valueBool == other.m_value.valueBool;
	case ValueType::Integer:
		return m_value.valueInteger == other.m_value.valueInteger;
	case ValueType::Number:
		return abs(m_value.valueNumber - other.m_value.valueNumber) < NUMBER_EQUALITY_TOLERANCE;
	case ValueType::String:
		return m_string == other.m_string;
	}
}



bool LuaValue::operator!=(const LuaValue& other) const
{
	return !(*this == other);
}




std::string LuaValue::ToString() const
{
	switch (m_type)
	{
	case ValueType::Nil:
		return "-Nil-";
	case ValueType::Bool:
		return m_value.valueBool ? "true" : "false";
	case ValueType::Integer:
		return std::to_string(m_value.valueInteger);
	case ValueType::Number:
		return std::to_string(m_value.valueNumber);
	case ValueType::String:
		return m_string;
	}
}

