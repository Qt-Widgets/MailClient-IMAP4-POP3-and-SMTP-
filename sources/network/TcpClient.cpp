#if defined(_WIN32) || defined(WIN32) || defined (_WIN64) || defined (WIN64)
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
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

#include "TcpClient.h"
#include "../utils/StringEx.h"

class TcpClientAttributes
{
public:
	bool				connected;
	unsigned long		socketFd;
	sockaddr_in			serverAddress;
	char*		 		serverName;
	int					serverPort;
	size_t				preFetchedBufferSize;
	unsigned char*		preFetchedBuffer;
	int					packetSize;
	unsigned char*		packet;
	char				packetDelimeter[32];
	bool				requireSSL;
	PacketBehaviour		phv;

	const SSL_METHOD*	SSLMethod;
	SSL_CTX*			SSLContext;
	SSL*				SSLSession;
};


TcpClient::TcpClient()
{
	implStructPtr = new TcpClientAttributes();
	memset(implStructPtr, 0, sizeof(TcpClientAttributes));

	implStructPtr->socketFd = 0;
	implStructPtr->connected = false;
	memset((void*)& implStructPtr->serverAddress, 0, sizeof(sockaddr_in));
	implStructPtr->preFetchedBufferSize = 0;
	implStructPtr->preFetchedBuffer = nullptr;
	implStructPtr->packetSize = 0;
	implStructPtr->packet = nullptr;
	implStructPtr->serverName = nullptr;
	implStructPtr->requireSSL = false;
	implStructPtr->phv = Undefined;

	implStructPtr->SSLMethod = nullptr;
	implStructPtr->SSLContext = nullptr;
	implStructPtr->SSLSession = nullptr;
}

TcpClient::TcpClient(int inSocket, bool requireSSL)
{
	implStructPtr = new TcpClientAttributes();
	memset(implStructPtr, 0, sizeof(TcpClientAttributes));

	implStructPtr->socketFd = inSocket;
	implStructPtr->connected = true;
	memset((void*)& implStructPtr->serverAddress, 0, sizeof(sockaddr_in));
	implStructPtr->preFetchedBufferSize = 0;
	implStructPtr->preFetchedBuffer = nullptr;
	implStructPtr->packetSize = 0;
	implStructPtr->packet = nullptr;
	implStructPtr->serverName = nullptr;
	implStructPtr->requireSSL = requireSSL;
	implStructPtr->phv = Undefined;

	//TODO - Must receive initialized SSL method, context and session
	implStructPtr->SSLMethod = nullptr;
	implStructPtr->SSLContext = nullptr;
	implStructPtr->SSLSession = nullptr;
}

TcpClient::~TcpClient()
{
	if (implStructPtr->connected)
	{
		CloseSocket();
	}

	if (implStructPtr->preFetchedBuffer != nullptr)
	{
		delete implStructPtr->preFetchedBuffer;
	}

	if (implStructPtr->packet != nullptr)
	{
		delete implStructPtr->packet;
	}

	if (implStructPtr->serverName != nullptr)
	{
		delete implStructPtr->serverName;
	}

	delete implStructPtr;
}

void TcpClient::SetPacketDelimeter(char * str)
{
	implStructPtr->phv = Delimited;
	memcpy(implStructPtr->packetDelimeter, str, strlen(str));
}

void TcpClient::SetPacketLength(long len)
{
	implStructPtr->phv = FixedLength;
	implStructPtr->packetSize = len;
}

bool TcpClient::CreateSocket(const char* servername, int serverport, bool reqSSL)
{
	implStructPtr->serverName = new char[strlen(servername) + 1];
	memset(implStructPtr->serverName, 0, strlen(servername) + 1);
	strcpy(implStructPtr->serverName, servername);

	implStructPtr->serverPort = serverport;
	implStructPtr->requireSSL = reqSSL;

	implStructPtr->serverAddress.sin_family = AF_INET;
	implStructPtr->serverAddress.sin_port = htons(serverport);
	u_long nRemoteAddr;

	char ipbuffer[32] = { 0 };
	memcpy(ipbuffer, servername, 31);

	bool ip = Network::IsIP4Address(ipbuffer);

	if (!ip)
	{
		hostent* pHE = gethostbyname(implStructPtr->serverName);
		if (pHE == 0)
		{
			nRemoteAddr = INADDR_NONE;
			return false;
		}
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
		implStructPtr->serverAddress.sin_addr.s_addr = nRemoteAddr;
	}
	else
	{
		inet_pton(AF_INET, implStructPtr->serverName, &implStructPtr->serverAddress.sin_addr);
	}

	if (implStructPtr->requireSSL)
	{
		if (SSL_library_init() < 0)
		{
			return false;
		}

		implStructPtr->SSLMethod = SSLv23_client_method();

		if ((implStructPtr->SSLContext = SSL_CTX_new(implStructPtr->SSLMethod)) == nullptr)
		{
			return false;
		}

		SSL_CTX_set_options(implStructPtr->SSLContext, SSL_OP_NO_SSLv2);

		implStructPtr->SSLSession = SSL_new(implStructPtr->SSLContext);

		implStructPtr->socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		SSL_set_fd(implStructPtr->SSLSession, implStructPtr->socketFd);
	}
	else
	{
		implStructPtr->socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (implStructPtr->socketFd == INVALID_SOCKET)
		{
			return false;
		}
	}
	return true;
}

bool TcpClient::CreateSocket(unsigned long inSocket, bool requireSSL)
{
	implStructPtr->requireSSL = requireSSL;
	implStructPtr->socketFd = inSocket;
	implStructPtr->connected = true;
	return true;
}

bool TcpClient::ConnectSocket(int &returncode)
{
	if (implStructPtr->connected == true)
	{
		return true;
	}

	returncode = connect(implStructPtr->socketFd, (sockaddr*)& implStructPtr->serverAddress, sizeof(sockaddr_in));

	if (returncode == SOCKET_ERROR)
	{
		returncode = socketerror;

		shutdown(implStructPtr->socketFd, 2);
		closesocket(implStructPtr->socketFd);
		implStructPtr->connected = false;

		return false;
	}

	if (implStructPtr->requireSSL)
	{
		if (SSL_connect(implStructPtr->SSLSession) != 1)
		{
			SSL_free(implStructPtr->SSLSession);
			shutdown(implStructPtr->socketFd, 2);
			closesocket(implStructPtr->socketFd);
			SSL_CTX_free(implStructPtr->SSLContext);
			implStructPtr->connected = false;

			return false;
		}
	}

	implStructPtr->connected = true;
	return true;
}

bool TcpClient::SwitchToSecureMode()
{
	implStructPtr->requireSSL = true;

	if (SSL_library_init() < 0)
	{
		return false;
	}

	implStructPtr->SSLMethod = SSLv23_client_method();

	if ((implStructPtr->SSLContext = SSL_CTX_new(implStructPtr->SSLMethod)) == nullptr)
	{
		return false;
	}

	SSL_CTX_set_options(implStructPtr->SSLContext, SSL_OP_NO_SSLv2);

	implStructPtr->SSLSession = SSL_new(implStructPtr->SSLContext);

	SSL_set_fd(implStructPtr->SSLSession, implStructPtr->socketFd);

	if (SSL_connect(implStructPtr->SSLSession) != 1)
	{
		SSL_free(implStructPtr->SSLSession);
		shutdown(implStructPtr->socketFd, 2);
		closesocket(implStructPtr->socketFd);
		SSL_CTX_free(implStructPtr->SSLContext);
		implStructPtr->connected = false;

		return false;
	}

	implStructPtr->connected = true;

	return true;
}

bool TcpClient::CloseSocket()
{
	if (implStructPtr->connected)
	{
		if (implStructPtr->requireSSL)
		{
			SSL_shutdown(implStructPtr->SSLSession);
			SSL_free(implStructPtr->SSLSession);
			shutdown(implStructPtr->socketFd, 0);
			closesocket(implStructPtr->socketFd);
			SSL_CTX_free(implStructPtr->SSLContext);
		}
		else
		{
			shutdown(implStructPtr->socketFd, 0);
			closesocket(implStructPtr->socketFd);
		}
	}
	implStructPtr->connected = false;

	return true;
}

bool TcpClient::ReceiveString(std::string &ioStr, const char *delimeter)
{
	char	buffer[1025] = { 0 };
	long	returnvalue;
	std::string	data;
	std::string  currentLine, nextLine;

	data.clear();

	if (implStructPtr->preFetchedBufferSize > 0)
	{
		if (strstr((char*)implStructPtr->preFetchedBuffer, delimeter) != 0)
		{
			std::string temp = (char*)implStructPtr->preFetchedBuffer;
			std::string tempdelim = delimeter;
			strsplit(temp, tempdelim, currentLine, nextLine);

			ioStr = currentLine;
			currentLine.clear();

			delete implStructPtr->preFetchedBuffer;
			implStructPtr->preFetchedBuffer = nullptr;
			implStructPtr->preFetchedBufferSize = nextLine.length();

			if (implStructPtr->preFetchedBufferSize > 0)
			{
				implStructPtr->preFetchedBuffer = new unsigned char[implStructPtr->preFetchedBufferSize + 1];
				memset(implStructPtr->preFetchedBuffer, 0, implStructPtr->preFetchedBufferSize + 1);
				memcpy(implStructPtr->preFetchedBuffer, nextLine.c_str(), implStructPtr->preFetchedBufferSize);
			}

			return true;
		}

		data = (char*)implStructPtr->preFetchedBuffer;
		implStructPtr->preFetchedBufferSize = 0;
		delete implStructPtr->preFetchedBuffer;
		implStructPtr->preFetchedBuffer = nullptr;
	}

	while (true)
	{
		memset(&buffer[0], 0, 1025);

		if (implStructPtr->requireSSL)
		{
			returnvalue = SSL_read(implStructPtr->SSLSession, &buffer[0], 1024);
		}
		else
		{
			returnvalue = recv(implStructPtr->socketFd, &buffer[0], 1024, 0);
		}

		if (returnvalue < 1)
		{
			int error = socketerror;
			ioStr.clear();
			implStructPtr->connected = false;
			return false;
		}

		data += buffer;

		if (strstr(data.c_str(), delimeter) != 0)
		{
			std::string tempdelim = delimeter;
			strsplit(data, tempdelim, currentLine, nextLine);

			implStructPtr->preFetchedBufferSize = nextLine.length();

			if (implStructPtr->preFetchedBufferSize > 0)
			{
				implStructPtr->preFetchedBuffer = new unsigned char[implStructPtr->preFetchedBufferSize + 1];
				memset(implStructPtr->preFetchedBuffer, 0, implStructPtr->preFetchedBufferSize + 1);
				memcpy(implStructPtr->preFetchedBuffer, nextLine.c_str(), implStructPtr->preFetchedBufferSize);
			}

			ioStr = currentLine;

			data.clear();
			currentLine.clear();
			return true;
		}
	}
	return true;
}

bool TcpClient::ReceiveString(std::string& ioStr)
{
	char	buffer[1025];
	long	returnvalue;
	std::string	data;

	memset(&buffer[0], 0, 1025);

	if (implStructPtr->requireSSL)
	{
		returnvalue = SSL_read(implStructPtr->SSLSession, &buffer[0], 1024);
	}
	else
	{
		returnvalue = recv(implStructPtr->socketFd, &buffer[0], 1024, 0);
	}

	if (returnvalue < 1)
	{
		int error = socketerror;
		ioStr.clear();
		implStructPtr->connected = false;
		return false;
	}

	ioStr = buffer;

	return true;
}

bool TcpClient::ReceiveBuffer(int len)
{
	char*	buffer = 0;
	long	bufferpos = 0;
	size_t	bytesread = 0;
	size_t	bytesleft = len;

	// If there are pre-fetched bytes left, we have to copy that first and relase memory

	if (implStructPtr->preFetchedBufferSize > 0)
	{
		if (implStructPtr->packet != nullptr)
		{
			delete implStructPtr->packet;
			implStructPtr->packet = nullptr;
		}
		implStructPtr->packet = new unsigned char[implStructPtr->preFetchedBufferSize];
		memcpy(implStructPtr->packet, implStructPtr->preFetchedBuffer, implStructPtr->preFetchedBufferSize);
		bytesleft = len - implStructPtr->preFetchedBufferSize;
		bufferpos = implStructPtr->preFetchedBufferSize;
		implStructPtr->preFetchedBufferSize = 0;
		delete implStructPtr->preFetchedBuffer;
		implStructPtr->preFetchedBuffer = nullptr;

		if (bytesleft < 1)
		{
			return true;
		}
	}

	while (true)
	{
		buffer = new char[bytesleft + 1];
		memset(buffer, 0, bytesleft + 1);

		if (implStructPtr->requireSSL)
		{
			bytesread =  SSL_read(implStructPtr->SSLSession, buffer, bytesleft);
		}
		else
		{
			bytesread = recv(implStructPtr->socketFd, buffer, bytesleft, 0);
		}

		if (bytesread < 1)
		{
			int error = socketerror;
			delete[] buffer;
			implStructPtr->packet = nullptr;
			len = 0;
			implStructPtr->connected = false;
			return false;
		}

		memcpy(implStructPtr->packet + bufferpos, buffer, bytesread);
		delete[] buffer;

		bufferpos = bufferpos + bytesread;

		bytesleft = bytesleft - bytesread;

		if (bufferpos >= len)
		{
			return true;
		}
	}
}

int TcpClient::PendingPreFetchedBufferSize()
{
	return implStructPtr->preFetchedBufferSize;
}


bool TcpClient::SendBuffer(const char* data, int &len)
{
	if (!implStructPtr->connected)
	{
		return false;
	}

	long sentsize = 0;

	if (implStructPtr->requireSSL)
	{
		sentsize = SSL_write(implStructPtr->SSLSession, data, len);
	}
	else
	{
		sentsize = send(implStructPtr->socketFd, data, len, 0);
	}

	if (sentsize == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool TcpClient::SendString(const std::string &str)
{
	int len = str.length();
	bool ret = SendBuffer(str.c_str(), len);
	return ret;
}

bool TcpClient::IsConnected()
{
	return implStructPtr->connected;
}

unsigned long TcpClient::GetSocket()
{
	return implStructPtr->socketFd;
}
