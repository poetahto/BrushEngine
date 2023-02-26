#pragma once

#include <unordered_set>

#define SID(s) (StringId(s))

class StringId
{
public:
	StringId(const char* string);

	operator const int() const;
	bool operator== (int other);
	bool operator== (StringId other);

private:
    const char* m_string;
    const int m_id;
};