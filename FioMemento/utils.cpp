//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdarg.h>
#include <string.h>

#include "utils.h"

std::string FormatV(const char* format, va_list argptr)
{
	va_list argptr2;
	va_copy(argptr2, argptr);
	std::string s;
	s.resize(vsnprintf(NULL, 0, format, argptr2) + 1, 0);
	va_end(argptr2);
	vsnprintf(&s[0], s.size(), format, argptr);
	if (s.length())
		s.pop_back();
	return s;
}

std::string Format(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	std::string s = FormatV(format, argptr);
	va_end(argptr);
	return s;
}

void StdOutV(LogLevel logLevel, const char* format, va_list argptr)
{
	switch (logLevel)
	{
	case LogLevel::Info:
		break;
	case LogLevel::Warning:
		break;
	case LogLevel::Error:
		break;
	default:
		fprintf(stdout, "ERROR: Unknown LogLevel : % i", logLevel);
		break;
	}
	std::string s = format;
	s += "\r\n";
	s = FormatV(s.data(), argptr);
	fprintf(stdout, s.data());
}

void StdOut(LogLevel logLevel, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	StdOutV(logLevel, format, argptr);
	va_end(argptr);
}

void StdOut(LogLevel logLevel, const std::string format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	StdOutV(logLevel, format.data(), argptr);
	va_end(argptr);
}

const char* getFileName(const char* file)
{
	const char* c = strrchr(file, '/');
	if (!c)
		c = strrchr(file, '\\');
	return c ? c + 1 : file;
}

void StdOutCurrentException(const char* file, int line, const char* function, const char* format, ...)
{
	try
	{
		std::rethrow_exception(std::current_exception());
	}
	catch (const Exception& e)
	{
		((Exception)e).StdOut();
	}
	catch (const std::exception& e)
	{
		StdOut(LogLevel::Error, e.what());
	}
	catch (...)
	{
		StdOut(LogLevel::Error, "Exception of unknown type.");
	}

	std::string m;
	if (format)
	{
		va_list argptr;
		va_start(argptr, format);
		m = FormatV(format, argptr);
		va_end(argptr);
	}
	::StdOut(LogLevel::Error, " => %s\r\n[%s, %i, %s()]", m.data(), getFileName(file), line, function);
}

Exception::Exception(const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	Message = FormatV(format, argptr);
	va_end(argptr);
}

void Exception::StdOut()
{
	if (File.length())
		::StdOut(LogLevel::Error, "%s\r\n[%s, %i, %s()]", Message.data(), File.data(), Line, Function.data());
	else
		::StdOut(LogLevel::Error, Message.data());
}

Exception2::Exception2(char* file, int line, char* function, int avErrorCode, const char* format, ...) :Exception()
{
	File = getFileName(file);
	Line = line;
	Function = function;

	va_list argptr;
	va_start(argptr, format);
	Message = FormatV(format, argptr);
	va_end(argptr);
	//appendAvError(avErrorCode);
}

Exception2::Exception2(char* file, int line, char* function, const char* format, ...) :Exception()
{
	File = getFileName(file);
	Line = line;
	Function = function;

	va_list argptr;
	va_start(argptr, format);
	Message = FormatV(format, argptr);
	va_end(argptr);
}