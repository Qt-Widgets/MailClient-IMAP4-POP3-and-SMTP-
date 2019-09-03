#ifndef _COMM_QUEUE
#define _COMM_QUEUE

#include <string>
#include <map>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../utils/Directory.h"
#include "../utils/StringEx.h"

#if defined(_WIN32) || defined(WIN32)
#include "mqueue.h"
#define O_RDONLY _O_RDONLY
#define O_APPEND _O_APPEND
#define O_WRONLY _O_WRONLY
#define O_RDWR _O_RDWR
#define O_CREAT _O_CREAT 
#else
#include <unistd.h>
#include <mqueue.h>
#endif

using namespace std;

class CommQueue
{
public:
	CommQueue();
    ~CommQueue();

    bool Open(const char* appname, bool create = false);
    bool Close();
    bool ReadLine(std::string &str);
	bool WriteLine(std::string& str);
private:
    std::string applicationName;
	mqd_t queueReference;

};

#endif
