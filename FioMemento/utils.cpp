//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <stdarg.h>

//extern "C" {
//#include <libavformat/avformat.h>
//}

#include "utils.h"

std::string FormatV(const char* format, va_list argptr)
{
	std::string s;
	s.resize(vsnprintf(NULL, 0, format, argptr) + 1, 0);
	vsnprintf(s.data(), s.size(), format, argptr);
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

//void StdOutV(LogLevel logLevel, const char* format, va_list argptr)
//{
//	int avLogLevel = AV_LOG_ERROR;
//	switch (logLevel)
//	{
//	case LogLevel::Info:
//		avLogLevel = AV_LOG_INFO;
//		break;
//	case LogLevel::Warning:
//		avLogLevel = AV_LOG_WARNING;
//		break;
//	case LogLevel::Error:
//		avLogLevel = AV_LOG_ERROR;
//		break;
//	default:
//		av_log(NULL, AV_LOG_ERROR, "Unknown LogLevel: %i", logLevel);
//		avLogLevel = AV_LOG_ERROR;
//		break;
//	}
//	std::string s = format;
//	s += "\r\n";
//	av_vlog(NULL, avLogLevel, s.c_str(), argptr);
//}

void StdOut(LogLevel logLevel, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	//StdOutV(logLevel, format, argptr);
	va_end(argptr);
}

void StdOut(LogLevel logLevel, const std::string format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	//StdOutV(logLevel, format.data(), argptr);
	va_end(argptr);
}

//std::string GetAvError(int avErrorCode)
//{
//	std::string m;
//	m.resize(AV_ERROR_MAX_STRING_SIZE + 1);
//	int rc = av_strerror(avErrorCode, m.data(), m.size());
//	if (rc)
//		sprintf_s(m.data(), m.size(), "-no description found-");
//	return m;
//}

char* getFileName(char* file)
{
	char* c = strrchr(file, '/');
	if (!c)
		c = strrchr(file, '\\');
	return c ? c + 1 : file;
}

void StdOutCurrentException(char* file, int line, char* function, const char* format, ...)
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

Exception::Exception(int avErrorCode, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	Message = FormatV(format, argptr);
	va_end(argptr);
	//appendAvError(avErrorCode);
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