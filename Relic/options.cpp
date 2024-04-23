//********************************************************************************************
//Developer: Sergiy Stoyan
//        s.y.stoyan@gmail.com
//        sergiy.stoyan@hotmail.com
//        http://www.cliversoft.com
//********************************************************************************************

#include <boost/spirit/home/support/detail/hold_any.hpp>

#include "options.h"

void StdOutConfiguration(po::variables_map vm, std::initializer_list<std::string> excludedOptions)
{
	std::cout << "Configuration:\r\n";
	for (const auto& it : vm)
	{
		if (std::find(excludedOptions.begin(), excludedOptions.end(), it.first) != excludedOptions.end())
			continue;
		std::cout << it.first.c_str() << "=";
		auto& value = it.second.value();
		if (auto v = boost::any_cast<long>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<ulong>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<int>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<uint>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<bool>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<double_t>(&value))
			std::cout << *v;
		else if (auto v = boost::any_cast<std::string>(&value))
			std::cout << *v;
		else
			std::cout << "<no cast>";
		std::cout << "\r\n";
	}
	//std::cout << "\r\n";
}