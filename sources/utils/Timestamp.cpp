#include "Timestamp.h"
#include <memory.h>
#include <string.h>

Timestamp::Timestamp()
{
    time_t rawtime;
    time ( &rawtime );
    timeinfo = *localtime(&rawtime);
}

Timestamp::Timestamp(time_t tinfo)
{
	timeinfo = *localtime(&tinfo);
}

Timestamp::Timestamp(const Timestamp& other)
{
    timeinfo = other.timeinfo;
}


Timestamp::Timestamp(const string str, const string format)
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

Timestamp::Timestamp(struct tm tinfo)
{
    timeinfo = tinfo;
}


void Timestamp::fromString(const string str, const string format, Timestamp &ts)
{
    ts.buildFromString(str,format);
}

void Timestamp::buildFromTm(const struct tm tmstruct)
{
	timeinfo = tmstruct;
}

void Timestamp::buildFromTimeT(time_t tinfo)
{
	timeinfo = *localtime(&tinfo);
}

void Timestamp::buildFromString(const string str, const string format)
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

Timestamp::~Timestamp()
{

}

Timestamp& Timestamp::operator=(const Timestamp& other)
{
    timeinfo = other.timeinfo;
    return *this;
}

bool Timestamp::operator!=( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1!=t2)
    {
        return true;
    }
    return false;
}

bool Timestamp::operator==( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1==t2)
    {
        return true;
    }
    return false;
}

bool Timestamp::operator>=( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1>=t2)
    {
        return true;
    }
    return false;
}

bool Timestamp::operator<=( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1<=t2)
    {
        return true;
    }
    return false;
}

bool Timestamp::operator>( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1>t2)
    {
        return true;
    }
    return false;
}

bool Timestamp::operator<( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    if(t1<t2)
    {
        return true;
    }
    return false;
}

Timestamp& Timestamp::operator+( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    t1 = t1+t2;

    timeinfo = *localtime(&t1);

    return *this;
}

Timestamp& Timestamp::operator-( const Timestamp& other)
{
    time_t t1 = mktime(&timeinfo);
    time_t t2 = mktime((tm*)&other.timeinfo);

    t1 = t1 - t2;

    timeinfo = *localtime(&t1);

    return *this;
}

std::string Timestamp::getDateString(const char *format)
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

std::string Timestamp::getDateString()
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


void Timestamp::addDays(int val)
{
    addSeconds(val*60*60*24);
}

void Timestamp::addHours(int val)
{
    addSeconds(val*60*60);
}

void Timestamp::addMinutes(int val)
{
    addSeconds(val*60);
}

void Timestamp::addSeconds(int val)
{
    // Commented due to GCC non POSIX behaviour
    //time_t t = mktime(&timeinfo);
    //t = t + val;
    //timeinfo = *localtime(&t);

    timeinfo.tm_sec = timeinfo.tm_sec + val;
    time_t t = mktime(&timeinfo);
    timeinfo = *localtime(&t);
}

int Timestamp::getDays()
{
    return timeinfo.tm_mday;
}

int Timestamp::getMonths()
{
    return timeinfo.tm_mon+1;
}

int Timestamp::getYears()
{
    return timeinfo.tm_year+1900;
}

int Timestamp::getHours()
{
    return timeinfo.tm_hour;
}

int Timestamp::getMinutes()
{
    return timeinfo.tm_min;
}

int Timestamp::getSeconds()
{
    return timeinfo.tm_sec;
}

const struct tm* Timestamp::getTimeStruct()
{
	return &timeinfo;
}

void Timestamp::setDay(int val)
{
    timeinfo.tm_mday = val;
}

void Timestamp::setMonth(int val)
{
    timeinfo.tm_mon = val-1;
}

void Timestamp::setYear(int val)
{
    timeinfo.tm_year = val-1900;
}

void Timestamp::setHour(int val)
{
    timeinfo.tm_hour = val;
}

void Timestamp::setMinute(int val)
{
    timeinfo.tm_min = val;
}

void Timestamp::setSecond(int val)
{
    timeinfo.tm_sec = val;
}
