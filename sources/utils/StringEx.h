#ifndef _STRING_HANDLER
#define _STRING_HANDLER

#include <string>
#include <list>
#include <vector>
#include <sstream>

using namespace std;

 void strsplit(const string &str, vector<string> &tokens, const string &delimiters = " ", bool trim = false);
 void strsplit(const string &str, vector<string> &tokens, char delim=' ', bool trim = false);
 void strsplit(const string &str, list<string> &tokens, const string &delimiters = " ", bool trim = false);
 void strsplit(const string &str, list<string> &tokens, char delim=' ', bool trim = false);
 void strsplit(const string &str, char delim, string &keystr, string &valuestr, bool trim = false);
 int strsplit(const string &str, const string &delim, string &keystr, string &valuestr, bool trim = false);

 void strreplace(string &srcstr, const string oldpattern, const string newpattern);
 void strreplace(string &srcstr, const string oldpattern, const char newchar);
 void strreplace(string &srcstr, const char oldchar, const char newchar);
 void strremove(string &srcstr, const char oldchar);

 void stralltrim(std::string &str);
 bool charisspace(int in);

 int strcharcount(const char *str, char ch);
 int strsubstringpos(const char* str,const char* substr);
 int strcharacterpos(const char* str,const char ch);

 bool strcontains(const char* str, const char* substr);
 bool strcontains(const char* str, const char ch);

 void strlower(string &srcstr);
 void strupper(string &srcstr);

 void strrealtostring(string &str, const double val);
 void strtimestamp(string &srcstr);

#endif

