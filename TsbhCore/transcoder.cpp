#include"transcoder.h"


void Transcoder:: unicodeToUTF8(const std::wstring &src, std::string& result)
{
	int n = WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, 0, 0, 0, 0 );
	result.resize(n);
	::WideCharToMultiByte( CP_UTF8, 0, src.c_str(), -1, (char*)result.c_str(), result.length(), 0, 0 );
}

void Transcoder::gb2312ToUnicode(const std::string& src, std::wstring& result)
{
	int n = MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, NULL, 0 );
	result.resize(n);
	::MultiByteToWideChar( CP_ACP, 0, src.c_str(), -1, (LPWSTR)result.c_str(), result.length());
}
void Transcoder::gb2312ToUtf8(const std::string& src, std::string& result)
{
	std::wstring strWideChar;
	gb2312ToUnicode(src, strWideChar);
	unicodeToUTF8(strWideChar, result);
}
QString Transcoder::str2qstr(const std::string str)
{
	return QString::fromLocal8Bit(str.data());  
}
std::string Transcoder::qstr2str(const QString & str)
{
	QByteArray cdata = str.toLocal8Bit();  
    return std::string(cdata);  
}


