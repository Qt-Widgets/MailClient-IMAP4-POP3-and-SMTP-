#if defined(_WIN32) || defined(WIN32) || defined (_WIN64) || defined (WIN64)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <memory.h>
#endif

#if defined(__gnu_linux__) || defined (__linux__)
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif

#if defined(__unix__)
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#endif

#include <stdlib.h>

#if defined(_WIN32) || defined(WIN32) || defined (_WIN64) || defined (WIN64)
#define socklen_t int
#define socketerror  ::WSAGetLastError()
#define socketioctl(socket, flag, var) ioctlsocket(socket, flag, (u_long*)&var)
#ifndef ERESTART
#define ERESTART 999
#endif
#endif

#if defined(__gnu_linux__) || defined (__linux__)
#define socketerror   errno
#define socketioctl(socket, flag, var) ioctlsocket(socket, flag, (char*)&var)
#define closesocket(n) close(n) 
#define SOCKET long
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#endif

#if defined(__unix__)
#define socketerror   errno
#define socketioctl(socket, flag, var) ioctl(socket, flag, (char*)&var)
#define closesocket(n) close(n) 
#define SOCKET long
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#ifndef ERESTART
#define ERESTART 999
#endif
#endif

#if defined(_WIN32) || defined(WIN32) || defined (_WIN64) || defined (WIN64)
#ifndef _SSL_APPLINK
#include <openssl/applink.c>
#endif
#endif

#include "Network.h"
#include <map>

Network networkselfinit;

struct timeval	timeout;

#if defined(_WIN32) || defined(WIN32) || defined (_WIN64) || defined (WIN64)
struct fd_set	master_set;
struct fd_set   working_set;
#else
fd_set	master_set;
fd_set   working_set;
#endif
long			max_sd;

std::map<SOCKET, Descriptor*> descriptorList;

Network::Network()
{
	#if defined(_WIN32) || defined(WIN32)
	WSACleanup();
	WSADATA WSData;
	long nRc = WSAStartup(0x0202, &WSData);
	if (nRc != 0)
	{
		return;
	}
	if (WSData.wVersion != 0x0202)
	{
		WSACleanup();
		return;
	}
	#endif

	FD_ZERO(&master_set);
	memset(&master_set, 0, sizeof(fd_set));

	timeout.tv_sec = 10 * 60;
	timeout.tv_usec = 0;
}

Network::~Network()
{
	#if defined(_WIN32) || defined(WIN32)
	WSACleanup();
	#endif
}

bool Network::IsIPAddress(char* str)
{
	//return (IsIP4Address(str) || IsIP6Address(str));
	return IsIP4Address(str);
}

bool Network::IsIP6Address(char*)
{
	return false;
}

bool Network::IsIP4Address(char* str)
{
	int slen = strlen(str);

	// Check the string length, for the range ...
	// 0.0.0.0 and 255.255.255.255
	if (slen < 7 || slen > 15)
	{
		// Bail out
		return false;
	}

	int ctr;
	bool isdelimeter = false;
	char nibble[4];
	memset((char*)&nibble[0], 0, 4);
	int nbindex = 0;
	for (ctr = 0; str[ctr] != '\0'; ctr++)
	{
		// Check for permitted characters
		if (str[ctr] != '.' && isdigit(str[ctr]) <= 0)
		{
			// Bail out
			return false;
		}

		// '.' Delimeter case
		if (str[ctr] == '.')
		{
			if (isdelimeter)
			{
				// The flag was set in last iteration
				// This means ".." type of expression was found
				// Bail out
				return false;
			}

			// We have read a complete nibble
			// The characters in the nibble must represent a permissible value
			int numval = atoi(nibble);
			if (numval < 0 || numval > 255)
			{
				return false;
			}

			// Set the flag and continue
			memset((char*)&nibble[0], 0, 4);
			nbindex = 0;
			isdelimeter = true;
			continue;
		}

		if (isdigit(str[ctr]) > 0)
		{
			isdelimeter = false;
			nibble[nbindex] = str[ctr];
			nbindex++;
			continue;
		}
	}

	return true;
}

void Network::GetLocalHostName(char *hostname)
{
	char name[128] = { 0 };
	int namelen = 128;
	if (gethostname(&name[0], namelen) == 0)
	{
		strcpy(hostname, name);
	}
}

void Network::GetLocalIPAddress(const int newServerfd, char *ipaddress)
{
	struct sockaddr_in localAddress;
	socklen_t addressLength = sizeof(localAddress);
	getsockname(newServerfd, (struct sockaddr*)&localAddress, &addressLength);
	strcpy(ipaddress, ::inet_ntoa(localAddress.sin_addr));
}

void Network::GetEndPoint(const char* url, char* host, int &port)
{
	/*
	if (dest_url[strlen(dest_url)] == '/')
		dest_url[strlen(dest_url)] = '\0';

	strncpy(proto, dest_url, (strchr(dest_url, ':') - dest_url));

	strncpy(hostname, strstr(dest_url, "://") + 3, sizeof(hostname));

	if (strchr(hostname, ':'))
	{
		tmp_ptr = strchr(hostname, ':');
		strncpy(portnum, tmp_ptr + 1, sizeof(portnum));
		*tmp_ptr = '\0';
	}

	port = atoi(portnum);

	if ((host = gethostbyname(hostname)) == NULL)
	{
		printf("Error: Cannot resolve hostname %s.\n", hostname);
		abort();
	}
	*/
}

bool Network::AddToDescriptors(void* sockref)
{
	int    on = 1;
	int    rc = -1;
	SOCKET sock = *(SOCKET*)sockref;
	rc = socketioctl(sock, FIONBIO, on);

	if (rc < 0)
	{
		return false;
	}

	FD_SET(sock, &master_set);

	if (sock > max_sd)
	{
		max_sd = sock;
	}

	return true;
}

bool Network::RemoveFromDescriptors(void* sockref)
{
	int    on = 1;
	int    rc = -1;
	SOCKET sock = *(SOCKET*)sockref;
	rc = socketioctl(sock, FIONBIO, on);

	if (rc < 0)
	{
		return false;
	}

	FD_CLR(sock, &master_set);

	if (sock == max_sd)
	{
		while (FD_ISSET(max_sd, &master_set) == 0)
			max_sd -= 1;
	}

	return true;
}

Descriptor* Network::GetActiveDescriptor()
{
	FD_ZERO(&working_set);
	int    rc = -1;
	memcpy(&working_set, &master_set, sizeof(master_set));

	rc = select(max_sd + 1, &working_set, NULL, NULL, &timeout);

	return nullptr;
}

