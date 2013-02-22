#pragma once

#include "brolib.h"


class Log
{
	bool endline;
	class EndLine{};

	void logString(char* format, ...);

public:
	Log();

	static EndLine newline;

	Log& operator <<(char* txt);
	Log& operator <<(int txt);
	Log& operator <<(const EndLine& endline);
};


