//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdarg.h>
#include <string.h>
#include <boost/exception/diagnostic_information.hpp> 

#include "utils.h"

std::string FormatV(const char* format, va_list argptr)
{
	std::string s;
	s.resize(4096);
	if (s.size() <= vsnprintf(&s[0], s.size(), format, argptr))
		StdOut(Warning, "The next message is trimmed!");
	/*//precise but less performance
	va_list argptr2;
	va_copy(argptr2, argptr);
	std::string s;
	s.resize(vsnprintf(NULL, 0, format, argptr2) + 1, 0);
	va_end(argptr2);
	vsnprintf(&s[0], s.size(), format, argptr);*/
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

void WriteV(_IO_FILE* file, LogLevel logLevel, const char* format, va_list argptr)
{
	std::string s;
	switch (logLevel)
	{
	case LogLevel::Info:
		break;
	case LogLevel::Warning:
		s.append("WARNING: ");
		break;
	case LogLevel::Error:
		s.append("ERROR: ");
		break;
	default:
		fprintf(file, "ERROR: Unknown LogLevel : % i", logLevel);
		break;
	}
	s.append(format);
	s += "\r\n";
	s = FormatV(s.c_str(), argptr);
	//(!)on POSIX, fprintf() is thread safe
	fprintf(file, s.c_str());
}

void Write(_IO_FILE* file, LogLevel logLevel, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	WriteV(file, logLevel, format, argptr);
	va_end(argptr);
}

void Write(_IO_FILE* file, LogLevel logLevel, const std::string format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	WriteV(file, logLevel, format.c_str(), argptr);
	va_end(argptr);
}

void StdOutV(LogLevel logLevel, const char* format, va_list argptr)
{
	WriteV(stdout, logLevel, format, argptr);
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
	StdOutV(logLevel, format.c_str(), argptr);
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
	/*catch (const std::exception& e)
	{
		StdOut(LogLevel::Error, e.what());
	}*/
	catch (const boost::exception& e)
	{
		StdOut(Error, boost::diagnostic_information(e));
	}
	catch (const std::exception& e)
	{
		StdOut(Error, boost::diagnostic_information(e));
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
	::StdOut(LogLevel::Error, " => %s\r\n[%s, %i, %s()]", m.c_str(), getFileName(file), line, function);
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
		::StdOut(LogLevel::Error, "%s\r\n[%s, %i, %s()]", Message.c_str(), File.c_str(), Line, Function.c_str());
	else
		::StdOut(LogLevel::Error, Message.c_str());
}

Exception2::Exception2(const char* file, int line, const char* function, int avErrorCode, const char* format, ...) :Exception()
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

Exception2::Exception2(const char* file, int line, const char* function, const char* format, ...) :Exception()
{
	File = getFileName(file);
	Line = line;
	Function = function;

	va_list argptr;
	va_start(argptr, format);
	Message = FormatV(format, argptr);
	va_end(argptr);
}