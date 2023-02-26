#include "StringId.h"
#include "Math.h"

StringId::StringId(const char* string) : 
	m_id     { Math::Hashing::hashCrc32(string, strlen(string)) },
	m_string { _strdup(string) }
{
}

StringId::operator const int() const
{
	return m_id;
}

bool StringId::operator== (int other)
{
	return m_id == other;
}

bool StringId::operator== (StringId other)
{
	return m_id == other.m_id;
}