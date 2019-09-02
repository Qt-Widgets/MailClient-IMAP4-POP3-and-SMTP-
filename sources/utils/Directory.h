#ifndef _DIRECTORY_UTIL
#define _DIRECTORY_UTIL

#if defined(_WIN32)  || defined(WIN32)
#define getcwd(ptr,n) _getcwd(ptr,n)
#define chdir(str) _chdir(str)
#if defined(_MSC_VER)
#define DIRECTORY_SEPARATOR '\\'
#else
#define DIRECTORY_SEPARATOR '/'
#endif
#include <Windows.h>
#include <direct.h>
#else
#define DIRECTORY_SEPARATOR '/'
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

#include <time.h>
#include <vector>
#include <string>

using namespace std;

class DirEntry
{
public:
	std::string Name;
	std::string FullPath;
	time_t CreationTime;
	time_t LastModifiedTime;
};

void dirgetparentdirectory(const std::string& str, std::string& pstr);
void dirgetextension(const std::string& str, std::string& ext);
void dirgetname(const std::string& fullpath, std::string& str);
void dircreatedirectory(const std::string& str);
bool dirisdirectory(const std::string& str);
bool dirfileexists(const std::string& str);
void dirgetdirectorylist(const std::string& dirname, std::vector<DirEntry>& dlist);
void dirgetfilelist(const std::string& dirname, std::vector<DirEntry>& dlist, const std::string& extension);
void dircurrentdirectory(std::string& str);
#endif

