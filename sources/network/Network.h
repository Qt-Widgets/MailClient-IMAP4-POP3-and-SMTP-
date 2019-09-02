#ifndef	_NETWORK
#define	_NETWORK

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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

typedef enum DescriptorType
{
	FDClient,
	FDServer
}DescriptorType;

typedef enum ServerState
{
	Running = 0,
	NormalExit = 1,
	CreateFailed = 2,
	BindFailed = 3,
	ListenFailed = 4
}ServerState;

typedef enum PacketBehaviour
{
	FixedLength = 0,
	Delimited = 1,
	Undefined = 2
}PacketBehaviour;

class Descriptor
{
public:
	virtual DescriptorType FDType() = 0;
};


class Network
{
public:
	Network();
	virtual ~Network();
	static bool IsIPAddress(char* str);
	static bool IsIP6Address(char* str);
	static bool IsIP4Address(char* str);
	static void GetLocalHostName(char *hostname);
	static void GetLocalIPAddress(const int newServerfd, char *ipaddress);
	static void GetEndPoint(const char* url, char* host, int &port);
	static bool AddToDescriptors(void* sockref);
	static bool RemoveFromDescriptors(void* sockref);
	static Descriptor* GetActiveDescriptor();
private:
};

#endif

