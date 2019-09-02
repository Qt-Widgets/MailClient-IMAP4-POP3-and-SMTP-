#include "DateTime.h"
#include <memory.h>
#include <string.h>

DateTime::DateTime()
{
    time_t rawtime;
    time ( &rawtime );
    timeinfo = *localtime(&rawtime);
}

DateTime::DateTime(time_t tinfo)
{
	timeinfo = *localtime(&tinfo);
}

DateTime::DateTime(const DateTime& other)
{
    timeinfo = other.timeinfo;
}

DateTime::DateTime(const string str, const string format)
{
    time_t rawtime;
    time ( &rawtime );
    timeinfo = *localtime(&rawtime);

    if(str.length()!=14)
    {
        if(str.length()!=12)
        {
            return;
        }
    }

    size_t pos;
    pos = format.find("yyyy");
    if(pos==string::npos)
    {
        pos = format.find("yy");
        if(pos!=string::npos)
        {
            timeinfo.tm_year = atoi(str.substr(pos,2).c_str())+100;
        }
    }
    else
    {
        timeinfo.tm_year = atoi(str.substr(pos,4).c_str())-1900;
    }

    pos = format.find("MM");
    if(pos!=string::npos)
    {
        timeinfo.tm_mon = atoi(str.substr(pos,2).c_str())-1;
    }

    pos = format.find("dd");
    if(pos!=string::npos)
    {
        timeinfo.tm_mday = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("hh");
    if(pos!=string::npos)
    {
        timeinfo.tm_hour = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("mm");
    if(pos!= string::npos)
    {
        timeinfo.tm_min = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("ss");
    if(pos!= string::npos)
    {
        timeinfo.tm_sec = atoi(str.substr(pos,2).c_str());
    }
}

DateTime::DateTime(struct tm tinfo)
{
    timeinfo = tinfo;
}

void DateTime::fromString(const string str, const string format, DateTime&ts)
{
    ts.buildFromString(str,format);
}

void DateTime::buildFromTm(const struct tm tmstruct)
{
	timeinfo = tmstruct;
}

void DateTime::buildFromTimeT(time_t tinfo)
{
	timeinfo = *localtime(&tinfo);
}

void DateTime::buildFromString(const string str, const string format)
{
    time_t rawtime;
    time ( &rawtime );
    timeinfo = *localtime(&rawtime);

    if(str.length()!=14)
    {
        if(str.length()!=12)
        {
            return;
        }
    }

    size_t pos;
    pos = format.find("yyyy");
    if(pos== string::npos)
    {
        pos = format.find("yy");
        if(pos!= string::npos)
        {
            timeinfo.tm_year = atoi(str.substr(pos,2).c_str())+100;
        }
    }
    else
    {
        timeinfo.tm_year = atoi(str.substr(pos,4).c_str())-1900;
    }

    pos = format.find("MM");
    if(pos!= string::npos)
    {
        timeinfo.tm_mon = atoi(str.substr(pos,2).c_str())-1;
    }

    pos = format.find("dd");
    if(pos!= string::npos)
    {
        timeinfo.tm_mday = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("hh");
    if(pos!= string::npos)
    {
        timeinfo.tm_hour = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("mm");
    if(pos!= string::npos)
    {
        timeinfo.tm_min = atoi(str.substr(pos,2).c_str());
    }

    pos = format.find("ss");
    if(pos!= string::npos)
    {
        timeinfo.tm_sec = atoi(str.substr(pos,2).c_str());
    }
}

DateTime::~DateTime()
{

}

DateTime& DateTime::operator=(const DateTime& other)
{
    timeinfo = other.timeinfo;
    return *this;
}

bool DateTime::operator!=( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1!=t2)
    {
        return true;
    }
    return false;
}

bool DateTime::operator==( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1==t2)
    {
        return true;
    }
    return false;
}

bool DateTime::operator>=( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1>=t2)
    {
        return true;
    }
    return false;
}

bool DateTime::operator<=( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1<=t2)
    {
        return true;
    }
    return false;
}

bool DateTime::operator>( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1>t2)
    {
        return true;
    }
    return false;
}

bool DateTime::operator<( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1<t2)
    {
        return true;
    }
    return false;
}

DateTime& DateTime::operator+( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    t1 = t1+t2;

    timeinfo = *localtime(&t1);

    return *this;
}

DateTime& DateTime::operator-( const DateTime& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    t1 = t1 - t2;

    timeinfo = *localtime(&t1);

    return *this;
}

std::string DateTime::getDateString(const char *format)
{
	string str = format;
    size_t pos = 0;
    bool ap = false;

    char buffer[256];
    memset((char*)&buffer[0],0,256);

    pos = str.find("ss");
    if(pos!= string::npos)
    {
        strreplace(str,"ss","%S");
    }

    pos = str.find("mm");
    if(pos!= string::npos)
    {
        strreplace(str,"mm","%M");
    }

    pos = str.find("hh");
    if(pos!= string::npos)
    {
        strreplace(str,"hh","%H");
    }
    else
    {   pos = str.find("h");
        if(pos!= string::npos)
        {
            strreplace(str,"h","%I");
            ap = true;
        }
    }

    pos = str.find("dd");
    if(pos!= string::npos)
    {
       strreplace(str,"dd","%d");
    }

    pos = str.find("MMMM");
    if(pos!= string::npos)
    {
       strreplace(str,"MMMM","%B");
    }
    else
    {
        pos = str.find("MM");
        if(pos!= string::npos)
        {
           strreplace(str,"MM","%m");
        }
    }

    pos = str.find("yyyy");
    if(pos!= string::npos)
    {
       strreplace(str,"yyyy","%Y");
    }
    else
    {
        pos = str.find("yy");
        if(pos!= string::npos)
        {
           strreplace(str,"yy","%y");
        }
    }

    if(ap)
    {
        str += "%p";

    }

	if(timeinfo.tm_year < 100)
	{
		timeinfo.tm_year += 100;
	}

    strftime(buffer,256,str.c_str(),&timeinfo);

    return buffer;
}

std::string DateTime::getDateString()
{
	string str = "yyyy/MM/dd hh:mm:ss";
    size_t pos = 0;
    bool ap = false;

    char buffer[256];
    memset((char*)&buffer[0],0,256);

    pos = str.find("ss");
    if(pos!= string::npos)
    {
       strreplace(str,"ss","%S");
    }

    pos = str.find("mm");
    if(pos!= string::npos)
    {
       strreplace(str,"mm","%M");
    }

    pos = str.find("hh");
    if(pos!= string::npos)
    {
       strreplace(str,"hh","%H");
    }
    else
    {   pos = str.find("h");
        if(pos!= string::npos)
        {
           strreplace(str,"h","%I");
            ap = true;
        }
    }

    pos = str.find("dd");
    if(pos!= string::npos)
    {
       strreplace(str,"dd","%d");
    }

    pos = str.find("MMMM");
    if(pos!= string::npos)
    {
       strreplace(str,"MMMM","%B");
    }
    else
    {
        pos = str.find("MM");
        if(pos!= string::npos)
        {
           strreplace(str,"MM","%m");
        }
    }

    pos = str.find("yyyy");
    if(pos!= string::npos)
    {
       strreplace(str,"yyyy","%Y");
    }
    else
    {
        pos = str.find("yy");
        if(pos!= string::npos)
        {
           strreplace(str,"yy","%y");
        }
    }

    if(ap)
    {
        str += "%p";

    }

	if(timeinfo.tm_year < 100)
	{
		timeinfo.tm_year += 100;
	}

    strftime(buffer,256,str.c_str(),&timeinfo);

    return buffer;
}


void DateTime::addDays(int val)
{
    addSeconds(val*60*60*24);
}

void DateTime::addHours(int val)
{
    addSeconds(val*60*60);
}

void DateTime::addMinutes(int val)
{
    addSeconds(val*60);
}

void DateTime::addSeconds(int val)
{
    // Commented due to GCC non POSIX behaviour
    //time_t t = mktime(&timeinfo);
    //t = t + val;
    //timeinfo = *localtime(&t);

    timeinfo.tm_sec = timeinfo.tm_sec + val;
    time_t t = mktime(&timeinfo);
    timeinfo = *localtime(&t);
}

int DateTime::getDays()
{
    return timeinfo.tm_mday;
}

int DateTime::getMonths()
{
    return timeinfo.tm_mon+1;
}

int DateTime::getYears()
{
    return timeinfo.tm_year+1900;
}

int DateTime::getHours()
{
    return timeinfo.tm_hour;
}

int DateTime::getMinutes()
{
    return timeinfo.tm_min;
}

int DateTime::getSeconds()
{
    return timeinfo.tm_sec;
}

const struct tm* DateTime::getTimeStruct()
{
	return &timeinfo;
}

void DateTime::setDay(int val)
{
    timeinfo.tm_mday = val;
}

void DateTime::setMonth(int val)
{
    timeinfo.tm_mon = val-1;
}

void DateTime::setYear(int val)
{
    timeinfo.tm_year = val-1900;
}

void DateTime::setHour(int val)
{
    timeinfo.tm_hour = val;
}

void DateTime::setMinute(int val)
{
    timeinfo.tm_min = val;
}

void DateTime::setSecond(int val)
{
    timeinfo.tm_sec = val;
}
