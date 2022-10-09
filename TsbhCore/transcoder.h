#pragma once
//ʹ�õ�Windows API��Linux����Ҫ��д����
#include <string>
#include <QString>
#include <Windows.h>
#include "rtsscore_global.h"

class RTSSCORE_EXPORT Transcoder
{
public:
	static void unicodeToUTF8(const std::wstring &src, std::string& result);
	static void gb2312ToUnicode(const std::string& src, std::wstring& result);
	static void gb2312ToUtf8(const std::string& src, std::string& result);
	static QString str2qstr(const std::string str);
	static std::string qstr2str(const QString & str);
};