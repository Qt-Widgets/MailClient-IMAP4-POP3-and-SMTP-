#ifndef _DATE_TIME
#define _DATE_TIME

#include "StringEx.h"
#include <time.h>

typedef enum TimeStampAttribute
{
    Days=0,
    Months=1,
    Years=2,
    Hours=3,
    Minutes=4,
    Seconds=5,
    Milliseconds

}TimeStampAttribute;

class Timestamp
{
public:
    Timestamp();
    Timestamp(const Timestamp& other);
    Timestamp(const string str, const string format);
    Timestamp(struct tm tinfo);
	Timestamp(time_t tinfo);
    ~Timestamp();

    Timestamp& operator=( const Timestamp& other);
    bool operator!=( const Timestamp& other);
    bool operator==( const Timestamp& other);
    bool operator>=( const Timestamp& other);
    bool operator<=( const Timestamp& other);
    bool operator>( const Timestamp& other);
    bool operator<( const Timestamp& other);
    Timestamp& operator+( const Timestamp& other);
    Timestamp& operator-( const Timestamp& other);

    void buildFromString(const std::string str, const std::string format);
	void buildFromTm(const struct tm tmstruct);
	void buildFromTimeT(time_t tinfo);

    void static fromString(const std::string str,const std::string format, Timestamp &ts);
    std::string getDateString(const char *format);
    std::string getDateString();
    void addDays(int val);
    void addHours(int val);
    void addMinutes(int val);
    void addSeconds(int val);
    int getDays();
    int getMonths();
    int getYears();
    int getHours();
    int getMinutes();
    int getSeconds();
	const struct tm* getTimeStruct();

    void setDay(int val);
    void setMonth(int val);
    void setYear(int val);
    void setHour(int val);
    void setMinute(int val);
    void setSecond(int val);
private:
    struct tm timeinfo;
};

#endif

