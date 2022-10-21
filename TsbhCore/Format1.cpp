
#include <algorithm>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include "Format1.h"
#include <string>



	/// Case insensitive string comparison for std::string
	int TFormat::caseCompare(const std::string &str1, const std::string &str2)
	{
		if (str1.empty())
			return str2.empty() ? 0 : -1;
		if (str2.empty())
			return 1;

		return _stricmp(str1.c_str(), str2.c_str());

	}

	/// Convert input string to lower-case
	std::string TFormat::lowerCase(const std::string &in)
	{
		std::string outString;
		outString.resize(in.size());
		std::transform(in.begin(), in.end(), outString.begin(), ::tolower);
		return outString;
	}

	/// Convert input string to upper-case
	std::string TFormat::upperCase(const std::string &in)
	{
		std::string outString;
		outString.resize(in.size());
		std::transform(in.begin(), in.end(), outString.begin(), ::toupper);
		return outString;
	}

	/// Case insensitive string find for std::string
	size_t TFormat::stringCaseFind(const std::string &str1, const std::string &str2)
	{
		const std::string& upStr1 = upperCase(str1);
		return upStr1.find(upperCase(str2));
	}



	/// Returns the extension of incoming string (lower-case by default), including the '.'
	std::string TFormat::getExtension(const std::string &inName, bool toLower)
	{
		if (inName.empty())
			return "";

		// convert to lower-case for insensitive comparison
		std::string outString = toLower ? lowerCase(inName) : inName;
		size_t found = outString.find_last_of(".");
		return (found != std::string::npos) ? outString.substr(found) : "";
	}

	/// Verifies the incoming string has the specified extension
	bool TFormat::hasExtension(const std::string& inName, std::string newExt)
	{
		std::transform(newExt.begin(), newExt.end(), newExt.begin(), ::tolower);
		return getExtension(inName) == newExt;
	}

