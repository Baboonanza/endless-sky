#include "LuaValue.h"

#include <utility>
#include <cassert>

using namespace std;



LuaValue::LuaValue(bool value)
{
	set(value);
}



LuaValue::~LuaValue()
{
	release();
}



LuaValue::LuaValue(int64_t value)
{
	set(value);
}



LuaValue::LuaValue(double value)
{
	set(value);
}



LuaValue::LuaValue(const std::string& value)
{
	set(value);
}



LuaValue::LuaValue(LuaValueMap&& value)
{
	set(std::move(value));
}



void LuaValue::set(bool value)
{
	release();
	m_type = ValueType::Bool;
	m_value.valueBool = value;
}



void LuaValue::set(int64_t value)
{
	release();
	m_type = ValueType::Int;
	m_value.valueInt = value;
}



void LuaValue::set(double value)
{
	release();
	m_type = ValueType::Double;
	m_value.valueDouble = value;
}



void LuaValue::set(const string& value)
{
	if (m_type == ValueType::String)
	{
		*m_value.valueString = value;
	}
	else
	{
		if (m_type == ValueType::ValueMap)
			delete m_value.valueMap;
		m_type = ValueType::String;
		m_value.valueString = new string(value);
	}
}



LuaValueMap& LuaValue::set(LuaValueMap&& value)
{
	if (m_type == ValueType::ValueMap)
	{
		*m_value.valueMap = std::move(value);
	}
	else
	{
		if (m_type == ValueType::String)
			delete m_value.valueString;
		m_type = ValueType::ValueMap;
		m_value.valueMap = new LuaValueMap(std::move(value));
	}
	return *m_value.valueMap;
}



LuaValue::ValueType LuaValue::getType()
{
	return ValueType();
}



bool LuaValue::isNil() const
{
	return m_type == ValueType::Nil;
}



bool LuaValue::getBool() const
{
	assert(m_type == ValueType::Bool);
	return m_value.valueBool;
}



int64_t LuaValue::getInt() const
{
	assert(m_type == ValueType::Int);
	return m_value.valueInt;
}



double LuaValue::getDouble() const
{
	assert(m_type == ValueType::Double);
	return m_value.valueDouble;
}



const std::string& LuaValue::getString() const
{
	assert(m_type == ValueType::String);
	return *m_value.valueString;
}



const LuaValueMap& LuaValue::getMap() const
{
	assert(m_type == ValueType::ValueMap);
	return *m_value.valueMap;
}



void LuaValue::release()
{
	if (m_type == ValueType::String)
		delete m_value.valueString;
	else if (m_type == ValueType::ValueMap)
		delete m_value.valueMap;
}
