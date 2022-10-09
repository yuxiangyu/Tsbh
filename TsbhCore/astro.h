#ifndef _CELENGINE_ASTRO_H_
#define _CELENGINE_ASTRO_H_
//************************************************
//ʱ�����
//************************************************
#include <iostream>
#include <string>
#include <math.h>
#include <windows.h>
#include "rtsscore_global.h"
using namespace std;
class RTSSCORE_EXPORT AeroTime
{
public:
	//���캯��
    AeroTime(void);
    AeroTime(int Y, int M, int D);
	AeroTime(int Y,int M,int D,int Hou,int Min,double Sec,string strtz);
	AeroTime(double);
	void double2At(double);

	//���ַ���ת��ΪAeroTime����
	static bool parseStkDate(const std::string&, AeroTime&);
	static bool parseDate(const std::string&, AeroTime&);
		
	//time_t���͵�double���͵�ת��
	static double TimeTToJulianDate(time_t ttime);

	static int stringtotime_t(const string &strDateStr,time_t &timeData);

	//���ã��õ�1970��1��1�������ĺ�����
		
	//�õ�һ����ʱ���double��ʾ
	double GetDoubleHour()
	{
		return hour + minute*1.0/60 + seconds*1.0/3600;
	}

    enum Format
    {
        Locale          = 0,
        TZName          = 1,
        UTCOffset       = 2,
    };
	//��ʱ��ת��Ϊ�ַ�����ʾ
    char* toCStr(Format format = Locale);
	char* toStkTimeStr(Format format = Locale);
	char* toShortStr(Format format = Locale);
		
	//��AeroTimeʱ��ת��Ϊ�����ձ�ʾ
    operator double() const;

public:
    int year;
    int month;
    int day;
    int hour;
    int minute;
	double seconds;
	//������һ�������е�λ�ã�������Ϊ0��������Ϊ6
    int wday;
    int utc_offset;
	//ʱ������
    std::string tzname;
};

inline double secsToDays(double s)
{
	return s * (1.0 / 86400.0);
}

inline double daysToSecs(double d)
{
	return d * 86400.0;
};
#endif 


