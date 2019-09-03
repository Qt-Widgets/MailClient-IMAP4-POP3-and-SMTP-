// mqueue.h
// Copyright 2016 Robin.Rowe@Cinepaint.org
// License open source MIT

#ifndef mqueue_h
#define mqueue_h

//#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#endif

#ifndef __cplusplus
#error
#endif

struct mq_attr
{	int mq_flags;
	int mq_maxmsg;
	int mq_msgsize;
	int mq_curmsgs;
};

typedef intptr_t mqd_t;

#if defined(_WIN32) || defined(WIN32)
#ifdef _MSC_VER
typedef unsigned int mode_t;
typedef long int ssize_t;
#endif
#endif

inline
int MultiByteToWideChar(LPCSTR lpMultiByteStr,LPWSTR lpWideCharStr,int bufsize)
{	
	UINT codePage=CP_UTF8;
	DWORD dwFlags=0;
	int cbMultiByte=-1;
	return MultiByteToWideChar(codePage,
		dwFlags,
		lpMultiByteStr, 
		cbMultiByte,
		lpWideCharStr,
		bufsize);
}

/* man mq_open:
Each message queue is identified by a name of the form '/somename'.
That is, a null-terminated string of up to NAME_MAX (i.e., 255)
characters consisting of an initial slash, followed by one or 
more characters, none of which are slashes. 
*/

inline
mqd_t mq_open(const char *name,int oflag,mode_t mode,mq_attr* attr=0)
{	
	const size_t bufsize=80;
	char pipeName[bufsize];
	#pragma warning(disable:4996)

	strcpy(pipeName,"\\\\.\\pipe\\");
	strcat(pipeName,name);
	if(strchr(pipeName,'/'))
	{	
		return -1;
	}

	HANDLE hPipe = NULL;
	DWORD err = 0;
	DWORD openMode = 0;

	if (oflag & O_CREAT)
	{
		switch (oflag)
		{
		default:
			return (mqd_t)-1;
		case O_CREAT | O_RDONLY:
			openMode = PIPE_ACCESS_INBOUND;
			break;
		case O_CREAT | O_WRONLY:
			openMode = PIPE_ACCESS_OUTBOUND;
			break;
		case O_CREAT | O_RDWR:
			openMode = PIPE_ACCESS_DUPLEX;
			break;
		}
	}

	if (attr)
	{
		DWORD pipeMode = PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT;
		DWORD kernelBufsize = 512;
		hPipe = CreateNamedPipeA(
			pipeName,
			openMode,
			pipeMode,
			PIPE_UNLIMITED_INSTANCES,
			kernelBufsize,
			kernelBufsize,
			NULL,
			NULL);

		if (hPipe == INVALID_HANDLE_VALUE)
		{	
			err = GetLastError();
			return -1;
		}

		BOOL ok = ConnectNamedPipe(hPipe, NULL);

		if (!ok)
		{
			err = GetLastError();
			return -1;
		}
	}
	else
	{
		hPipe = CreateFileA(
			pipeName,    
			GENERIC_READ |   
			GENERIC_WRITE,
			0,               
			NULL,           
			OPEN_EXISTING,   
			0,               
			NULL);           

		if (hPipe == INVALID_HANDLE_VALUE)
		{
                    err = GetLastError();
                    return -1;
		}

		BOOL fSuccess;

		DWORD dwMode = PIPE_READMODE_MESSAGE;

		fSuccess = SetNamedPipeHandleState(
			hPipe,     
			&dwMode,   
			NULL,      
			NULL);    

		if (!fSuccess)
		{
			err = GetLastError();
			return -1;
		}
	}

	return (mqd_t)hPipe;
}

inline
int mq_send(mqd_t mqdes, const char *buffer,size_t bufsize, unsigned msg_prio)
{	
	if(msg_prio!=0)
	{	
		return -1;
	}

	HANDLE hPipe = (HANDLE) mqdes;
	DWORD cbWritten; 
	DWORD err;
    const BOOL ok = WriteFile(hPipe,buffer,(DWORD)bufsize,&cbWritten,NULL);

	if (!ok || cbWritten == 0)
	{
		FlushFileBuffers(hPipe);
		DisconnectNamedPipe(hPipe);
	}

	if (!ok) 
	{	
		err = GetLastError();
		return -1;
	} 
	return cbWritten;
}

inline
ssize_t mq_receive(mqd_t mqdes,char* buffer,size_t bufsize,unsigned* msg_prio)
{   
	if(msg_prio!=0)
	{	
		return -1;
	}
	HANDLE hPipe = (HANDLE) mqdes;
	DWORD numRead; 
	DWORD err;
	const BOOL ok = ReadFile(hPipe,buffer,(DWORD) bufsize,&numRead,NULL);

	if(!ok)
	{	
		err = GetLastError();

		if (err != ERROR_MORE_DATA || numRead == 0)
		{
			FlushFileBuffers(hPipe);
			DisconnectNamedPipe(hPipe);
			return -1;
		}
	}

	return numRead;
}

inline
int mq_close(mqd_t mqdes)
{	
	if(mqdes < 0)
	{	
		return -1;
	}
    
	const BOOL ok = CloseHandle((HANDLE)mqdes);
	
	return ok ? 0:-1;
}

inline
int mq_unlink(const char* name)
{	
	(void) name;
	return 0;
}

#endif
