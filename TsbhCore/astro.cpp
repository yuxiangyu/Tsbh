// astro.cpp
#include <time.h>
#define DLL_CLASS __declspec(dllexport)
#include "astro.h"


static const char* MonthAbbrList[12] =
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

double /*CSTK::BASELINE::*/secondsToJulianDate(double sec)
{
    return sec / 86400.0;
}

double /*CSTK::BASELINE::*/julianDateToSeconds(double jd)
{
    return jd * 86400.0;
}

//角分秒表示的角度和小数表示的角度的互换

AeroTime::AeroTime()
{
    year = 0;
    month = 0;
    day = 0;
    hour = 0;
    minute = 0;
    seconds = 0.0;
    wday = 0;
    utc_offset = 0;
    tzname = "UTC";
}

AeroTime::AeroTime(int Y, int M, int D)
{
    year = Y;
    month = M;
    day = D;
    hour = 0;
    minute = 0;
    seconds = 0.0;
    wday = 0;
    utc_offset = 0;
    tzname = "UTC";
}

AeroTime::AeroTime(int Y,int M,int D,int Hou,int Min,double Sec,string strtz)
{
	year = Y;
	month = M;
	day = D;
	hour = Hou;
	minute = Min;
	seconds = Sec;	
	wday = 0;
	utc_offset = 0;
	tzname = "UTC";
}
//儒勒日转换到日历时间
AeroTime::AeroTime(double jd)
{
    bool error=true;
	_int64 a = (_int64) floor(jd + 0.5);
	wday = (a + 1) % 7;
	if (wday < 0) wday += 7;
	double c;
	if (a < 2299161)
	{
		c = (double) (a + 1524);
	}
	else
	{
		double b = (double) ((_int64) floor((a - 1867216.25) / 36524.25));
		c = a + b - (_int64) floor(b / 4) + 1525;
	}

	_int64 d = (_int64) floor((c - 122.1) / 365.25);
	_int64 e = (_int64) floor(365.25 * d);
	_int64 f = (_int64) floor((c - e) / 30.6001);

	double dday = c - e - (_int64) floor(30.6001 * f) + ((jd + 0.5) - a);

	// This following used to be 14.0, but gcc was computing it incorrectly, so
	// it was changed to 14
	month = (int) (f - 1 - 12 * (_int64) (f / 14));
	year = (int) (d - 4715 - (_int64) ((7.0 + month) / 10.0));
	day = (int) dday;

	double dhour = (dday - day) * 24;
	hour=(int)dhour;
	

	double dminute = (dhour - hour) * 60;
	//if (dminute>59.9999)
	//{
	//	dminute=0;
	//	hour++;
	//}
	//if (dminute<0.00001)
	//{
	//	dminute=0;
	//}
	minute = (int) dminute;

	seconds = (dminute - minute) * 60;
	utc_offset = 0;

	if (month < 1 || month > 12 || hour > 23 || minute > 59 || seconds >= 60.0 || seconds < 0.0)
		error=true;
	else
		error=false;

	
    tzname = "UTC";
}


void AeroTime::double2At(double jd)
{


}

char* AeroTime::toStkTimeStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%02d %s %04d %02d:%02d:%f", 
		//	day, MonthAbbrList[month-1], year, 
		//	hour, minute, seconds);
		sprintf(date,  "%02d %s %04d %02d:%02d:%f", 
			day, MonthAbbrList[month-1], year, 
			hour, minute, seconds);
		break;
	}
	return date;

}

char* AeroTime::toShortStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	switch(format)
	{
	case Locale:
	case TZName:
		//_snprintf(date, sizeof(date), "%d/%d/%d %d:%d:%0.2f", 
		//	year, month,day,hour, minute, seconds);
		sprintf(date,"%0.2f",seconds);
		if(atoi(date)==60)
		{
			minute++;
			seconds=0;
		}
		sprintf(date,  "%d/%d/%d %d:%d:%0.2f", 
			year, month,day,hour, minute, seconds);
		break;
	}
	return date;

}



// 日历时间转换为儒勒日
AeroTime::operator double() const
{
    int y = year, m = month;
    if (month <= 2)
    {
        y = year - 1;
        m = month + 12;
    }
    int B = -2;
    if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
    {
        B = y / 400 - y / 100;
    }

    return (floor(365.25 * y) +
            floor(30.6001 * (m + 1)) + B + 1720996.5 +
            day + hour / 24.0 + minute / 1440.0 + seconds / 86400.0);
}


// 日历时间规整为字符形式表达。
bool AeroTime::parseStkDate(const string& s, AeroTime& date)
{
	int year = 0;
	unsigned int month = 1;
	unsigned int day = 1;
	unsigned int hour = 0;
	unsigned int minute = 0;
	double second = 0.0;
	char a_month[3] = {0};

	//2 Nov 2014 19:45:57.141465445
	if (sscanf(s.c_str(), "%u %s %d %u:%u:%lf",
		&day,&a_month,&year, &hour, &minute, &second) == 6)
	{
		//a_month = MonthAbbrList[1];
		//printf("mon = %s\n",MonthAbbrList[1]);
		for (int i = 1;i<13;i++)
		{
			if (MonthAbbrList[i - 1][0] == a_month[0]&&MonthAbbrList[i - 1][1] == a_month[1]&&MonthAbbrList[i - 1][2] == a_month[2])
			{
				month = i;
			}			
		}

		if (month < 1 || month > 12)
			return false;
		if (hour > 23 || minute > 59 || second >= 60.0 || second < 0.0)
			return false;

		// Days / month calculation . . .
		int maxDay = 31 - ((0xa50 >> month) & 0x1);
		if (month == 2)
		{
			// Check for a leap year
			if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
				maxDay = 29;
			else
				maxDay = 28;
		}
		if (day > (unsigned int) maxDay || day < 1)
			return false;

		date.year = year;
		date.month = month;
		date.day = day;
		date.hour = hour;
		date.minute = minute;
		date.seconds = second;

		return true;
	}

	return false;
}

// 日历时间规整为字符形式表达。
bool AeroTime::parseDate(const string& s, AeroTime& date)
{
    int year = 0;
    unsigned int month = 1;
    unsigned int day = 1;
    unsigned int hour = 0;
    unsigned int minute = 0;
    double second = 0.0;
	char a_month[3] = {0};

	//2 Nov 2014 19:45:57.141465445
    if (sscanf(s.c_str(), "%d %u %u %u:%u:%lf ",
               &year, &month, &day, &hour, &minute, &second) == 6 ||
		sscanf(s.c_str(), "%d %s %u %u:%u:%lf",
		       &year,&a_month,&day, &hour, &minute, &second) == 6 ||
		sscanf(s.c_str(), "%u %s %d %u:%u:%lf",
			   &day,&a_month,&year, &hour, &minute, &second) == 6 ||
        sscanf(s.c_str(), " %d %u %u %u:%u ",
               &year, &month, &day, &hour, &minute) == 5 ||
        sscanf(s.c_str(), " %d %u %u ", &year, &month, &day) == 3)
    {
		//a_month = MonthAbbrList[1];
		//printf("mon = %s\n",MonthAbbrList[1]);
		for (int i = 1;i<13;i++)
		{
			if (MonthAbbrList[i - 1][0] == a_month[0]&&MonthAbbrList[i - 1][1] == a_month[1]&&MonthAbbrList[i - 1][2] == a_month[2])
			{
				month = i;
			}			
		}
		
        if (month < 1 || month > 12)
            return false;
        if (hour > 23 || minute > 59 || second >= 60.0 || second < 0.0)
            return false;

        // Days / month calculation . . .
        int maxDay = 31 - ((0xa50 >> month) & 0x1);
        if (month == 2)
        {
            // Check for a leap year
            if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
                maxDay = 29;
            else
                maxDay = 28;
        }
        if (day > (unsigned int) maxDay || day < 1)
            return false;

        date.year = year;
        date.month = month;
        date.day = day;
        date.hour = hour;
        date.minute = minute;
        date.seconds = second;

        return true;
    }

    return false;
}


double AeroTime::TimeTToJulianDate(time_t ttime)
{
	tm* t = localtime(&ttime);
 	int year = t->tm_year + 1900;
 	int month = t->tm_mon + 1;
 	int day = t->tm_mday ;
 	int hour = t->tm_hour;
 	int minute = t->tm_min;
 	int seconds = t->tm_sec;
	AeroTime atime = AeroTime(year,month,day,hour,minute,(double)seconds,"UTC");
 	double jdtime = (double)atime;
 	return jdtime;


}

int AeroTime::stringtotime_t(const string &strDateStr,time_t &timeData)
{
    char *pBeginPos = (char*) strDateStr.c_str();
    char *pPos = strstr(pBeginPos,"-");
    if(pPos == NULL)
    {
        printf("strDateStr[%s] err \n", strDateStr.c_str());
        return -1;
    }
    int iYear = atoi(pBeginPos);
    int iMonth = atoi(pPos + 1);
    pPos = strstr(pPos + 1,"-");
    if(pPos == NULL)
    {
        printf("strDateStr[%s] err \n", strDateStr.c_str());
        return -1;
    }
    int iDay = atoi(pPos + 1);
    int iHour=0;
    int iMin=0;
    int iSec=0;
    pPos = strstr(pPos + 1," ");
    //为了兼容有些没精确到时分秒的
    if(pPos != NULL)
    {
        iHour=atoi(pPos + 1);
        pPos = strstr(pPos + 1,":");
        if(pPos != NULL)
        {
            iMin=atoi(pPos + 1);
            pPos = strstr(pPos + 1,":");
            if(pPos != NULL)
            {
                iSec=atoi(pPos + 1);
            }
        }
    }

    struct tm sourcedate;
    //bzero((void*)&sourcedate,sizeof(sourcedate));
    sourcedate.tm_sec = iSec;
    sourcedate.tm_min = iMin; 
    sourcedate.tm_hour = iHour;
    sourcedate.tm_mday = iDay;
    sourcedate.tm_mon = iMonth - 1; 
    sourcedate.tm_year = iYear - 1900;
    timeData = mktime(&sourcedate);  
    return 0;
}
char* AeroTime::toCStr(Format format)
{
	//char date[255];
	char *date;
	date=new char[255];
	if((int)ceil(seconds)==60)
	{
		seconds=0;
		minute++;
	}
	if(minute==60)
	{
		minute=0;
		hour++;
	}
    switch(format)
    {
    case Locale:
    case TZName:
        //_snprintf(date, sizeof(date), "%04d %s %02d %02d:%02d:%02d %s", 
        //         year, MonthAbbrList[month-1], day, 
         //        hour, minute, (int)ceil(seconds), tzname.c_str());
		sprintf(date,  "%04d %s %02d %02d:%02d:%02d %s", 
			year, MonthAbbrList[month-1], day, 
			hour, minute, (int)ceil(seconds), tzname.c_str());
        break;
    case UTCOffset:
        {
            int sign = utc_offset < 0 ? -1:1;
            int h_offset = sign * utc_offset / 3600;
            int m_offset = (sign * utc_offset - h_offset * 3600) / 60;
            //_snprintf(date, sizeof(date), "%04d %s %02d %02d:%02d:%02d %c%02d%02d", 
            //        year, MonthAbbrList[month-1], day, 
            //        hour, minute, (int)ceil(seconds), (sign==1?'+':'-'), h_offset, m_offset);
			sprintf(date,  "%04d %s %02d %02d:%02d:%02d %c%02d%02d", 
				year, MonthAbbrList[month-1], day, 
				hour, minute, (int)ceil(seconds), (sign==1?'+':'-'), h_offset, m_offset);
        }
        break;
    }
    return date;
}



