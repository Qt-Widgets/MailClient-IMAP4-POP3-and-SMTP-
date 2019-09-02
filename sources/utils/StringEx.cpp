#include "StringEx.h"
#include <string.h>
#include <memory.h>
#include <ctype.h>
#include <time.h>

int strsubstringpos(const char* str,const char* substr)
{
    char* pdest = (char*)strstr( str, substr );
    if(pdest == 0)
    {
        return -1;
    }
    ptrdiff_t result = pdest - str;
    return (int) result;
}

int	strcharacterpos(const char* str, const char ch)
{
	for (int ctr = 0; str[ctr] != '\0'; ctr++)
	{
		if (str[ctr] == ch)
		{
			return ctr;
		}
	}
	return -1;
}

bool strcontains(const char* str, const char* substr)
{
	if (strsubstringpos(str, substr) > -1)
	{
		return true;
	}

	return false;
}

bool strcontains(const char* str, const char ch)
{
	if (strcharacterpos(str, ch) > -1)
	{
		return true;
	}

	return false;
}

void strsplit(const string &str, vector<string> &tokens, const string &delimiters, bool trim)
{
    // Skip delimiters at beginning
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first non-delimiter
    string::size_type pos = str.find(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
		std::string item = str.substr(lastPos, pos - lastPos);

		if (trim)
		{
			stralltrim(item);
		}

		// Found a token, add it to the vector
		tokens.push_back(item);

        // Skip delimiters
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next non-delimiter
        pos = str.find(delimiters, lastPos);
    }
}

void strsplit(const string &str, vector<string> &tokens, char delim, bool trim)
{
    stringstream ss(str); //convert string to stream
    string item;

    while(getline(ss, item, delim))
    {
		if (trim)
		{
			stralltrim(item);
		}

        tokens.push_back(item); //add token to vector
    }
}

void strsplit(const string &str, list<string> &tokens, const string &delimiters, bool trim)
{
    // Skip delimiters at beginning
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);

    // Find first non-delimiter
    string::size_type pos = str.find(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
		std::string item = str.substr(lastPos, pos - lastPos);

		if (trim)
		{
			stralltrim(item);
		}

        // Found a token, add it to the vector
        tokens.push_back(item);

        // Skip delimiters
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next non-delimiter
        pos = str.find(delimiters, lastPos);
    }
}

void strsplit(const string &str, list<string> &tokens, char delim, bool trim)
{
    stringstream ss(str); //convert string to stream
    string item;

    while(getline(ss, item, delim))
    {
		if (trim)
		{
			stralltrim(item);
		}

        tokens.push_back(item); //add token to vector
    }
}


void strsplit(const string &str, char delim, string &keystr, string &valuestr, bool trim)
{
    stringstream ss(str); //convert string to stream
    string item;

    int ctr = 0;

    while(getline(ss, item, delim))
    {
        if(ctr==0)
        {
            keystr = item;
            ctr++;
            continue;
        }

        if(ctr==1)
        {
            valuestr = item;
            ctr++;
            continue;
        }

        if(ctr>1)
        {
            valuestr += delim;
            valuestr += item;
            ctr++;
        }
    }
}

int strsplit(const string &str, const string &delim, string &keystr, string &valuestr, bool trim)
{
    int pos = (int) str.find(delim);

    if(pos == -1)
    {
        return pos;
    }

    char *tptr = new char[(size_t)pos+1];//bug
    memset(tptr,0, (size_t)pos+1);

    memcpy(tptr, str.c_str(), pos);

    keystr = tptr;

    delete [] tptr;

    tptr = NULL;

    const char *ptr = str.c_str();

    valuestr = &ptr[pos+delim.length()];

    return pos;
}

void strreplace(string &srcstr, const char oldchar, const char newchar)
{
    size_t len = srcstr.length();
    for(size_t ctr = 0 ; ctr < len ; ctr++)
    {
        if(srcstr[ctr] == oldchar)
        {
            srcstr[ctr] = newchar;
        }
    }
}


void  strreplace(string &srcstr, const string oldpattern, const string newpattern)
{
    if (oldpattern.length() == 0 || srcstr.length() == 0)
    {
        return;
    }

    size_t idx = 0;

    for(;;)
    {
        idx = srcstr.find( oldpattern, idx);

        if (idx == string::npos)
        {
            break;
        }

        srcstr.replace( idx, oldpattern.length(), newpattern);
        idx += newpattern.length();
    }
}

void strreplace(string &srcstr, const string oldpattern, const char newchar)
{
	char buff[2]={0};
	buff[0] = newchar;
	strreplace(srcstr, oldpattern, buff);
}

void stralltrim(std::string &str)
{
	size_t buff_len = str.length();

	buff_len++;

    char* buffer = new char[buff_len];//bug
    memset(buffer, 0, buff_len);
    strcpy(buffer, str.c_str());

    size_t len = strlen(buffer);

	if (len < 1)
	{
		delete[] buffer;
		return;
	}

    for(long i = len-1;  i > -1 ; i--)
    {
		if (i < 0)
		{
			break;
		}
		
		if(!isspace(buffer[i]))
        {
            break;
        }
        buffer[i] = '\0';
    }

    len = strlen(buffer);

    if(len<1)
    {
        str = buffer;
		delete[] buffer;
        return;
    }

    const char *buf=(const char*)&buffer[0];

    for ( NULL; *buf && isspace(*buf); buf++);

    str = buf;
	delete[] buffer;
}


bool charisspace(int in)
{
   if ((in == 0x20) || (in >= 0x09 && in <= 0x0D)) return true;
   return false;
}

int strcharcount(const char *str, char ch)
{
    int c=0;
    for(int i=0; str[i] != '\0' ;i++)
        if(str[i]==ch)
            c++;

    return c;
}

void strlower(string &srcstr)
{
    size_t len = srcstr.length();
    for(size_t ctr = 0 ; ctr < len ; ctr++)
    {
        srcstr[ctr] = tolower(srcstr[ctr]);
    }
}

void strupper(string &srcstr)
{
    size_t len = srcstr.length();
    for(size_t ctr = 0 ; ctr < len ; ctr++)
    {
        srcstr[ctr] = toupper(srcstr[ctr]);
    }
}

void strremove(string &srcstr, const char oldchar)
{
	size_t buff_len = srcstr.length();

	buff_len++;

    char* buffer = new char[buff_len];//bug
    memset(buffer, 0, buff_len);

    int strctr = 0;
    int bufctr = 0;

    for( ; strctr < buff_len -1 ; strctr++)
    {
        if(srcstr[strctr] != oldchar)
        {
            buffer[bufctr] = srcstr[strctr];
            bufctr++;
        }
    }

    srcstr = buffer;
    delete [] buffer;
}


void strrealtostring(string &str, const double val)
{
    char ptr[255];
    memset((void*)&ptr[0],0,255);
    sprintf(ptr,"%10.6lf",val);
    str = ptr;
}

void strtimestamp(string &srcstr)
{
    time_t rawtime;
    time(&rawtime);
    tm timeinfo = *localtime(&rawtime);

    std::string str = "yyyyMMddhhmmss";
    size_t pos = 0;
    bool ap = false;

    char buffer[256];
    memset((char*)&buffer[0], 0, 256);

    pos = strsubstringpos(str.c_str(), "ss");
    if (pos != -1)
    {
        strreplace(str, "ss", "%S");
    }

    pos = strsubstringpos(str.c_str(), "mm");
    if (pos != -1)
    {
        strreplace(str, "mm", "%M");
    }

    pos = strsubstringpos(str.c_str(), "hh");
    if (pos != -1)
    {
        strreplace(str, "hh", "%H");
    }
    else
    {
        pos = strsubstringpos(str.c_str(), "h");
        if (pos != -1)
        {
            strreplace(str, "h", "%I");
            ap = true;
        }
    }

    pos = strsubstringpos(str.c_str(), "dd");
    if (pos != -1)
    {
        strreplace(str, "dd", "%d");
    }

    pos = strsubstringpos(str.c_str(), "MMMM");
    if (pos != -1)
    {
        strreplace(str, "MMMM", "%B");
    }
    else
    {
        pos = strsubstringpos(str.c_str(), "MM");
        if (pos != -1)
        {
            strreplace(str, "MM", "%m");
        }
    }

    pos = strsubstringpos(str.c_str(), "yyyy");
    if (pos != -1)
    {
        strreplace(str, "yyyy", "%Y");
    }
    else
    {
        pos = strsubstringpos(str.c_str(), "yy");
        if (pos != -1)
        {
            strreplace(str, "yy", "%y");
        }
    }

    if (ap)
    {
        str += "%p";

    }

    if (timeinfo.tm_year < 100)
    {
        timeinfo.tm_year += 100;
    }

    strftime(buffer, 256, str.c_str(), &timeinfo);

    srcstr = buffer;
}
