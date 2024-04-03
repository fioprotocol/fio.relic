#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <chrono>

//extern "C" {
//#include "cJSON.h"
//}
//
#include "utils.h"

using namespace std::chrono;
class Progresser
{
public:

	Progresser(int fileCount, milliseconds showProgressInterval = milliseconds(300), int showProgressFactor = 100)
	{
		Progresser::fileCount = fileCount;
		currentFileIndex = 0;
		Progresser::showProgressInterval = showProgressInterval;
		Progresser::nextShowProgressTime = milliseconds(0);
		totalProgress = 0;
		Progresser::showProgressFactor = showProgressFactor;
	}

	void NextFile()
	{
		currentFileIndex++;
	}

	void OnFileProgress(float progress)
	{
		milliseconds time = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
		if (time < nextShowProgressTime)
			return;
		totalProgress = (float)(currentFileIndex - 1 + progress) / fileCount;
		printf("{{\"currentFileIndex\": %i, \"progress\": %i %%}}\n", currentFileIndex, (int)(totalProgress * showProgressFactor));
		fflush(stdout);
		nextShowProgressTime = time + showProgressInterval;
	}

	void Finish()
	{
		if (totalProgress < 1)
		{
			nextShowProgressTime = milliseconds(0);
			OnFileProgress(1);
		}
	}

protected:
	int fileCount;
	int currentFileIndex;
	milliseconds showProgressInterval;
	milliseconds nextShowProgressTime;
	float totalProgress;
	int showProgressFactor;
};

int main(int argc, char** argv)
{
	std::set_terminate([]() { STDOUT_CURRENT_EXCEPTION(NULL); std::abort(); });

	StdOut(LogLevel::Info, "VERSION: %s %s", APP_NAME, APP_VERSION);

	if (argc < 2)
	{
		//StdOutUsage();
		return 0;
	}

	try
	{
	}
	catch (Exception e)
	{
		e.StdOut();
	}
	catch (...)
	{
		STDOUT_CURRENT_EXCEPTION(NULL);
	}

	return 0;
}
