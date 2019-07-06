#pragma once

inline int first_tok(char *str)
{
	return atoi(strtok(str, ","));
}
inline int next_tok()
{
	char *pch = strtok(NULL, ",");
	if (pch)
	{
		return atoi(pch);
	}
	else
	{
		LOG_LV1("CONFIG", "ERROR: Expected valid token, got null");
		return 0;
	}
}

inline void append_int_comma(char *buffer, int x)
{
	itoa(x, buffer + strlen(buffer), 10);
	strcat(buffer, ",");
}
