#include "Logger.h"
#include <stdarg.h>
#include <memory.h>
#include <string.h>

#if defined(_WIN32) || defined(WIN32)
#include <process.h>
#define getpid()	_getpid()
#define pid_t    long
#else
#include <unistd.h> 
#endif

Logger objLogger;

Logger*  Logger::GetInstance()
{
    return &objLogger;
}

Logger::Logger()
{
    logDirectory = "";
    logFileSize = 1024;
    logFile = NULL;

    char pidstr[16];
    memset((char*)&pidstr[0],0,16);
    sprintf(pidstr,"%d",getpid());
    moduleName = pidstr;

    logLevelMap.clear();

    logLevelMap[LOG_INFO]       ="Information";
    logLevelMap[LOG_WARNING]    ="Warning    ";
    logLevelMap[LOG_ERROR]      ="Error      ";
    logLevelMap[LOG_CRITICAL]   ="Critical   ";
    logLevelMap[LOG_PANIC]      ="Panic      ";
}

Logger::~Logger()
{
    StopLogging();
}

void Logger::StopLogging()
{
    if(logFile!=NULL)
    {
        fflush(logFile);
        fclose(logFile);
    }
    logLevelMap.clear();
}

void Logger::CreateBackupFileName(std::string &str)
{
    Timestamp ts;
	std::string tstamp = ts.getDateString("yyyy.MM.dd-hh.mm.ss");
    char temp[1024];
    memset((char*)&temp[0],0,16);
    sprintf(temp,"%s_%s.log",moduleName.c_str(),tstamp.c_str());
    str = temp;
}

void Logger::StartLogging()
{
    if(logDirectory.empty() || logDirectory.length()<1)
    {
		std::string parent_dir, current_dir;
		dircurrentdirectory(current_dir);
		dirgetparentdirectory(current_dir, parent_dir);

		logDirectory = parent_dir + "/log/";

        if(!dirisdirectory(logDirectory))
        {
            dircreatedirectory(logDirectory);
        }
    }

    logFilename = logDirectory + moduleName + ".log";

    logFile = fopen(logFilename.c_str(),"w+");
}

void Logger::Write(std::string logEntry, LogLevel llevel, const char* func, const char* file, int line)
{
    if(logFile!=NULL)
    {
        int sz = ftell(logFile);

        if(sz >= logFileSize*1024)
        {
			std::string temp;
            CreateBackupFileName(temp);
			std::string backupfile = logBackupDirectory + temp;
            StopLogging();
            int res = rename(logFilename.c_str(),backupfile.c_str());
            StartLogging();
        }

		std::string sourcefile;
		dirgetname(file, sourcefile);
		std::string lvel = logLevelMap[llevel];

        Timestamp ts;
		std::string tstamp = ts.getDateString("yyyy.MM.dd-hh.mm.ss");
        char temp[1024];
        memset((char*)&temp[0],0,16);

        char fname[256]={0};
        memcpy(fname,func,255);
        #if defined(_WIN32) || defined(WIN32)
        #else
        int pos = strcharacterpos(fname,'(');
        fname[pos]=0;
        #endif

		std::string left, right;

        strsplit(fname, "::", left, right);
        if(right.length()>1)
        {
            strcpy(fname,right.c_str());
        }

        strsplit(fname, " ", left, right);
        if(right.length()>1)
        {
            strcpy(fname,right.c_str());
        }

        sprintf(temp,"%s|%s|%05d|%s|%s| ",tstamp.c_str(),lvel.c_str(),line,fname,sourcefile.c_str());

        logEntry = temp + logEntry;
        fprintf(logFile,"%s\n",logEntry.c_str());
        fflush(logFile);
    }
}

void Logger::SetModuleName(const char *mname)
{
    int len = strlen(mname);

    int ctr = 0;

    int pos1 = 0;
    int pos2 = 0;

    pos1 = strcharacterpos(mname, '/');
    pos2 = strcharacterpos(mname, '\\');

    if(pos1 > -1 || pos2 > -1)
    {
        for(ctr = len; ; ctr--)
        {
            if(mname[ctr] == '/' || mname[ctr] == '\\')
            {
                break;
            }
        }
        char buffer[33]={0};

        strncpy((char*)&buffer[0], (char*)&mname[ctr+1], 32);

        moduleName = buffer;
    }
    else
    {
        moduleName = mname;
    }

    strreplace(moduleName, ".exe", "");
    strreplace(moduleName, ".EXE", "");
}

void Logger::SetLogFileSize(int flsz)
{
    logFileSize = flsz;
}

void Logger::SetLogDirectory(std::string &dirpath)
{
    logDirectory = dirpath;

    char buffer[2048]={0};

    strcpy(buffer, logDirectory.c_str());

    if(buffer[strlen(buffer)-1]== '/' || buffer[strlen(buffer)-1]== '\\')
    {
        buffer[strlen(buffer)-1] = 0;
    }

    strcat(buffer, ".bak/");

    logBackupDirectory = buffer;

    if(!dirisdirectory(buffer))
    {
        dircreatedirectory(buffer);
    }
}

void Logger::WriteExtended(LogLevel llevel, const char *func, const char *file, int line, const char* format,...)
{
    char tempbuf[1024];
    memset((char*)&tempbuf[0],0,1024);
    va_list args;
    va_start(args, format);
    vsprintf(tempbuf, format, args);
    tempbuf[1023]=0;
    Write(tempbuf,llevel,func,file,line);
}


