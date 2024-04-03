//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <stdio.h>

#define THROW_Exception2(...) throw Exception2(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__)

#define STDOUT_CURRENT_EXCEPTION(format, ...) StdOutCurrentException(__FILE__, __LINE__, __FUNCTION__, format, __VA_ARGS__)
void StdOutCurrentException(char* file, int line, char* function, const char* format = NULL, ...);

enum LogLevel
{
	Info,
	Warning,
	Error
};

//void StdOutV(LogLevel logLevel, const char* format, va_list argptr);
void StdOut(LogLevel logLevel, const char* format, ...);
void StdOut(LogLevel logLevel, const std::string format, ...);
std::string FormatV(const char* format, va_list argptr);
std::string Format(const char* format, ...);
//std::string GetAvError(int avErrorCode);

#include <iostream>
class Exception : public std::exception
{
public:
	Exception(int avErrorCode, const char* format, ...);
	Exception(const char* format, ...);

	void StdOut();

	std::string Message;
	std::string File;
	int Line = 0;
	std::string Function;

protected:
	Exception() {}

	/*inline void appendAvError(int avErrorCode)
	{
		Message += " AvError: " + GetAvError(avErrorCode);
	}*/
};

class Exception2 : public Exception
{
public:
	Exception2(char* file, int line, char* function, int avErrorCode, const char* format, ...);
	Exception2(char* file, int line, char* function, const char* format, ...);

protected:
};

#endif //UTILS_H
