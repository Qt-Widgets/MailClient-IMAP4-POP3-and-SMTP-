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
#endif

#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

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

#include "TcpClient.h"
#include "Network.h"
#include "../utils/StringEx.h"

class SocketReference
{
public:
	bool				connected;
	unsigned long		socketFd;
	sockaddr_in			serverAddress;
	std::string 		serverName;
	int					serverPort;
	size_t				preFetchedBufferSize;
	unsigned char*		preFetchedBuffer;
	int					packetSize;
	unsigned char*		packet;
	char				packetDelimeter[32];
	bool				requireSSL;
	PacketBehaviour		phv;

	BIO*				certificateBIO;
	X509*               certificate;
	X509_name_st*		certificateName;
	const SSL_METHOD*	SSLMethod;
	SSL_CTX*			SSLContext;
	SSL*				SSLSession;
	std::string 		certificateNamePrintable;

	SocketReference()
	{
		connected = false;
		socketFd = -1;
		memset(&serverAddress, 0, sizeof(sockaddr_in));
		serverName = "";
		serverPort = -1;
		preFetchedBufferSize = 0;
		preFetchedBuffer = nullptr;
		packetSize = 0;
		packet = nullptr;
		memset(&packetDelimeter[0], 0, 32);
		requireSSL = false;
		phv = Delimited;

		certificateBIO = nullptr;
		certificate = nullptr;
		certificateName = nullptr;
		SSLMethod = nullptr;
		SSLContext = nullptr;
		SSLSession = nullptr;
		certificateNamePrintable = "";
	}
};

TcpClient::TcpClient()
{
	socketReferencePtr = new SocketReference();
	socketReferencePtr->socketFd = 0;
	socketReferencePtr->connected = false;
	memset((void*)&socketReferencePtr->serverAddress, 0, sizeof(sockaddr_in));
	socketReferencePtr->preFetchedBufferSize = 0;
	socketReferencePtr->preFetchedBuffer = nullptr;
	socketReferencePtr->packetSize = 0;
	socketReferencePtr->packet = nullptr;
	socketReferencePtr->requireSSL = false;
	socketReferencePtr->phv = Undefined;

	socketReferencePtr->certificateBIO = nullptr;
	socketReferencePtr->certificate = nullptr;
	socketReferencePtr->certificateName = nullptr;
	socketReferencePtr->SSLMethod = nullptr;
	socketReferencePtr->SSLContext = nullptr;
	socketReferencePtr->SSLSession = nullptr;
}

TcpClient::TcpClient(int inSocket, bool requireSSL)
{
	socketReferencePtr = new SocketReference();
	socketReferencePtr->socketFd = inSocket;
	socketReferencePtr->connected = true;
	memset((void*)&socketReferencePtr->serverAddress, 0, sizeof(sockaddr_in));
	socketReferencePtr->preFetchedBufferSize = 0;
	socketReferencePtr->preFetchedBuffer = nullptr;
	socketReferencePtr->packetSize = 0;
	socketReferencePtr->packet = nullptr;
	socketReferencePtr->requireSSL = requireSSL;
	socketReferencePtr->phv = Undefined;

	socketReferencePtr->certificateBIO = nullptr;
	socketReferencePtr->certificate = nullptr;
	socketReferencePtr->certificateName = nullptr;
	socketReferencePtr->SSLMethod = nullptr;
	socketReferencePtr->SSLContext = nullptr;
	socketReferencePtr->SSLSession = nullptr;
}

TcpClient::TcpClient(const TcpClient& other)
{
	socketReferencePtr = new SocketReference();
	memset((void*)&socketReferencePtr->serverAddress, 0, sizeof(sockaddr_in));

	socketReferencePtr->preFetchedBufferSize = 0;

	if (socketReferencePtr->preFetchedBuffer != nullptr)
	{
		delete socketReferencePtr->preFetchedBuffer;
		socketReferencePtr->preFetchedBuffer = nullptr;
	}

	socketReferencePtr->packetSize = 0;

	if (socketReferencePtr->packet != nullptr)
	{
		delete socketReferencePtr->packet;
		socketReferencePtr->packet = nullptr;
	}

	socketReferencePtr->socketFd = other.socketReferencePtr->socketFd;
	socketReferencePtr->connected = other.socketReferencePtr->connected;
	memcpy((void*)&socketReferencePtr->serverAddress, (void*)&other.socketReferencePtr->serverAddress, sizeof(sockaddr_in));

	if (other.socketReferencePtr->preFetchedBufferSize > 0)
	{
		socketReferencePtr->preFetchedBuffer = new unsigned char[other.socketReferencePtr->preFetchedBufferSize];
		socketReferencePtr->preFetchedBufferSize = other.socketReferencePtr->preFetchedBufferSize;
		memcpy((unsigned char*)&socketReferencePtr->preFetchedBuffer, (unsigned char*)&other.socketReferencePtr->preFetchedBuffer, socketReferencePtr->preFetchedBufferSize);
	}

	if (other.socketReferencePtr->packetSize > 0)
	{
		socketReferencePtr->packet = new unsigned char[other.socketReferencePtr->packetSize];
		socketReferencePtr->packetSize = other.socketReferencePtr->packetSize;
		memcpy((unsigned char*)&socketReferencePtr->packet, (unsigned char*)&other.socketReferencePtr->packet, socketReferencePtr->packetSize);
	}

	socketReferencePtr->phv = other.socketReferencePtr->phv;

	socketReferencePtr->requireSSL = other.socketReferencePtr->requireSSL;

	socketReferencePtr->certificateBIO = other.socketReferencePtr->certificateBIO;
	socketReferencePtr->certificate = other.socketReferencePtr->certificate;
	socketReferencePtr->certificateName = other.socketReferencePtr->certificateName;
	socketReferencePtr->SSLMethod = other.socketReferencePtr->SSLMethod;
	socketReferencePtr->SSLContext = other.socketReferencePtr->SSLContext;
	socketReferencePtr->SSLSession = other.socketReferencePtr->SSLSession;
}

TcpClient& TcpClient::operator=(const TcpClient& other)
{
	memset((void*)&socketReferencePtr->serverAddress, 0, sizeof(sockaddr_in));

	socketReferencePtr->preFetchedBufferSize = 0;

	if (socketReferencePtr->preFetchedBuffer != nullptr)
	{
		delete socketReferencePtr->preFetchedBuffer;
		socketReferencePtr->preFetchedBuffer = nullptr;
	}

	socketReferencePtr->packetSize = 0;

	if (socketReferencePtr->packet != nullptr)
	{
		delete socketReferencePtr->packet;
		socketReferencePtr->packet = nullptr;
	}

	socketReferencePtr->socketFd = other.socketReferencePtr->socketFd;
	socketReferencePtr->connected = other.socketReferencePtr->connected;
	memcpy((void*)&socketReferencePtr->serverAddress, (void*)&other.socketReferencePtr->serverAddress, sizeof(sockaddr_in));

	if (other.socketReferencePtr->preFetchedBufferSize > 0)
	{
		socketReferencePtr->preFetchedBuffer = new unsigned char[other.socketReferencePtr->preFetchedBufferSize];
		socketReferencePtr->preFetchedBufferSize = other.socketReferencePtr->preFetchedBufferSize;
		memcpy((unsigned char*)&socketReferencePtr->preFetchedBuffer, (unsigned char*)&other.socketReferencePtr->preFetchedBuffer, socketReferencePtr->preFetchedBufferSize);
	}

	if (other.socketReferencePtr->packetSize > 0)
	{
		socketReferencePtr->packet = new unsigned char[other.socketReferencePtr->packetSize];
		socketReferencePtr->packetSize = other.socketReferencePtr->packetSize;
		memcpy((unsigned char*)&socketReferencePtr->packet, (unsigned char*)&other.socketReferencePtr->packet, socketReferencePtr->packetSize);
	}

	socketReferencePtr->phv = other.socketReferencePtr->phv;

	socketReferencePtr->requireSSL = other.socketReferencePtr->requireSSL;

	socketReferencePtr->certificateBIO = other.socketReferencePtr->certificateBIO;
	socketReferencePtr->certificate = other.socketReferencePtr->certificate;
	socketReferencePtr->certificateName = other.socketReferencePtr->certificateName;
	socketReferencePtr->SSLMethod = other.socketReferencePtr->SSLMethod;
	socketReferencePtr->SSLContext = other.socketReferencePtr->SSLContext;
	socketReferencePtr->SSLSession = other.socketReferencePtr->SSLSession;
	return *this;
}

TcpClient::~TcpClient()
{
	CloseSocket();
	while (IsConnected() == true)
	{
	}

	if (socketReferencePtr != nullptr)
	{
		if (socketReferencePtr->preFetchedBuffer != nullptr)
		{
			delete socketReferencePtr->preFetchedBuffer;
		}

		delete socketReferencePtr;
	}
}

void TcpClient::SetPacketDelimeter(char * str)
{
	socketReferencePtr->phv = Delimited;
	memcpy(socketReferencePtr->packetDelimeter, str, strlen(str));
}

void TcpClient::SetPacketLength(long len)
{
	socketReferencePtr->phv = FixedLength;
	socketReferencePtr->packetSize = len;
}

bool TcpClient::CreateSocket(const char* servername, int serverport, bool requireSSL)
{
	socketReferencePtr->serverName = servername;
	socketReferencePtr->serverPort = serverport;
	socketReferencePtr->requireSSL = requireSSL;

	socketReferencePtr->serverAddress.sin_family = AF_INET;
	socketReferencePtr->serverAddress.sin_port = htons(serverport);
	u_long nRemoteAddr;

	char ipbuffer[32] = { 0 };
	memcpy(ipbuffer, servername, 31);

	bool ip = Network::IsIP4Address(ipbuffer);

	if (!ip)
	{
		hostent* pHE = gethostbyname(socketReferencePtr->serverName.c_str());
		if (pHE == 0)
		{
			nRemoteAddr = INADDR_NONE;
			return false;
		}
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
		socketReferencePtr->serverAddress.sin_addr.s_addr = nRemoteAddr;
	}
	else
	{
		inet_pton(AF_INET, socketReferencePtr->serverName.c_str(), &socketReferencePtr->serverAddress.sin_addr);
	}

	if (socketReferencePtr->requireSSL)
	{
		socketReferencePtr->certificateBIO = BIO_new(BIO_s_file());

		if (SSL_library_init() < 0)
		{
			return false;
		}

		socketReferencePtr->SSLMethod = SSLv23_client_method();

		if ((socketReferencePtr->SSLContext = SSL_CTX_new(socketReferencePtr->SSLMethod)) == nullptr)
		{
			return false;
		}

		SSL_CTX_set_options(socketReferencePtr->SSLContext, SSL_OP_NO_SSLv2);

		socketReferencePtr->SSLSession = SSL_new(socketReferencePtr->SSLContext);

		socketReferencePtr->socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		SSL_set_fd(socketReferencePtr->SSLSession, socketReferencePtr->socketFd);
	}
	else
	{
		socketReferencePtr->socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (socketReferencePtr->socketFd == INVALID_SOCKET)
		{
			return false;
		}
	}
	return true;
}

bool TcpClient::CreateSocket(unsigned long inSocket, bool requireSSL)
{
	socketReferencePtr->requireSSL = requireSSL;
	socketReferencePtr->socketFd = inSocket;
	socketReferencePtr->connected = true;
	return true;
}

bool TcpClient::ConnectSocket(int &returncode)
{
	if (socketReferencePtr->connected == true)
	{
		return true;
	}

	returncode = connect(socketReferencePtr->socketFd, (sockaddr*)&socketReferencePtr->serverAddress, sizeof(sockaddr_in));

	if (returncode == SOCKET_ERROR)
	{
		returncode = socketerror;

		shutdown(socketReferencePtr->socketFd, 2);
		closesocket(socketReferencePtr->socketFd);
		socketReferencePtr->connected = false;

		return false;
	}

	if (socketReferencePtr->requireSSL)
	{
		if (SSL_connect(socketReferencePtr->SSLSession) != 1)
		{
			SSL_free(socketReferencePtr->SSLSession);
			shutdown(socketReferencePtr->socketFd, 2);
			closesocket(socketReferencePtr->socketFd);
			X509_free(socketReferencePtr->certificate);
			SSL_CTX_free(socketReferencePtr->SSLContext);
			socketReferencePtr->connected = false;

			return false;
		}

		socketReferencePtr->certificate = SSL_get_peer_certificate(socketReferencePtr->SSLSession);

		if (socketReferencePtr->certificate != nullptr)
		{
			socketReferencePtr->certificateName = X509_NAME_new();
			socketReferencePtr->certificateName = X509_get_subject_name(socketReferencePtr->certificate);
			socketReferencePtr->certificateNamePrintable = X509_NAME_oneline(socketReferencePtr->certificateName, 0, 0);
		}
	}

	socketReferencePtr->connected = true;
	return true;
}

bool TcpClient::SwitchToSecureMode()
{
	socketReferencePtr->requireSSL = true;

	socketReferencePtr->certificateBIO = BIO_new(BIO_s_file());

	if (SSL_library_init() < 0)
	{
		return false;
	}

	socketReferencePtr->SSLMethod = SSLv23_client_method();

	if ((socketReferencePtr->SSLContext = SSL_CTX_new(socketReferencePtr->SSLMethod)) == nullptr)
	{
		return false;
	}

	SSL_CTX_set_options(socketReferencePtr->SSLContext, SSL_OP_NO_SSLv2);

	socketReferencePtr->SSLSession = SSL_new(socketReferencePtr->SSLContext);

	SSL_set_fd(socketReferencePtr->SSLSession, socketReferencePtr->socketFd);

	if (SSL_connect(socketReferencePtr->SSLSession) != 1)
	{
		SSL_free(socketReferencePtr->SSLSession);
		shutdown(socketReferencePtr->socketFd, 2);
		closesocket(socketReferencePtr->socketFd);
		X509_free(socketReferencePtr->certificate);
		SSL_CTX_free(socketReferencePtr->SSLContext);
		socketReferencePtr->connected = false;

		return false;
	}

	socketReferencePtr->certificate = SSL_get_peer_certificate(socketReferencePtr->SSLSession);

	if (socketReferencePtr->certificate != nullptr)
	{
		socketReferencePtr->certificateName = X509_NAME_new();
		socketReferencePtr->certificateName = X509_get_subject_name(socketReferencePtr->certificate);
		socketReferencePtr->certificateNamePrintable = X509_NAME_oneline(socketReferencePtr->certificateName, 0, 0);
	}

	socketReferencePtr->connected = true;

	return true;
}

bool TcpClient::CloseSocket()
{
	if (socketReferencePtr != nullptr)
	{
		if (socketReferencePtr->connected)
		{
			if (socketReferencePtr->requireSSL)
			{
				SSL_shutdown(socketReferencePtr->SSLSession);
				SSL_free(socketReferencePtr->SSLSession);
				shutdown(socketReferencePtr->socketFd, 0);
				closesocket(socketReferencePtr->socketFd);
				X509_free(socketReferencePtr->certificate);
				SSL_CTX_free(socketReferencePtr->SSLContext);
			}
			else
			{
				shutdown(socketReferencePtr->socketFd, 0);
				closesocket(socketReferencePtr->socketFd);
			}
		}
		socketReferencePtr->connected = false;
	}
	return false;
}

bool TcpClient::ReceiveString(std::string &ioStr, const char *delimeter)
{
	char	buffer[1025] = { 0 };
	long	returnvalue;
	std::string	data;
	std::string  currentLine, nextLine;

	data.clear();

	if (socketReferencePtr->preFetchedBufferSize > 0)
	{
		if (strstr((char*)socketReferencePtr->preFetchedBuffer, delimeter) != 0)
		{
			std::string temp = (char*)socketReferencePtr->preFetchedBuffer;
			std::string tempdelim = delimeter;
			strsplit(temp, tempdelim, currentLine, nextLine);

			ioStr = currentLine;
			currentLine.clear();

			delete socketReferencePtr->preFetchedBuffer;
			socketReferencePtr->preFetchedBuffer = nullptr;
			socketReferencePtr->preFetchedBufferSize = nextLine.length();

			if (socketReferencePtr->preFetchedBufferSize > 0)
			{
				socketReferencePtr->preFetchedBuffer = new unsigned char[socketReferencePtr->preFetchedBufferSize + 1];
				memset(socketReferencePtr->preFetchedBuffer, 0, socketReferencePtr->preFetchedBufferSize + 1);
				memcpy(socketReferencePtr->preFetchedBuffer, nextLine.c_str(), socketReferencePtr->preFetchedBufferSize);
			}

			return true;
		}

		data = (char*)socketReferencePtr->preFetchedBuffer;
		socketReferencePtr->preFetchedBufferSize = 0;
		delete socketReferencePtr->preFetchedBuffer;
		socketReferencePtr->preFetchedBuffer = nullptr;
	}

	while (true)
	{
		memset(&buffer[0], 0, 1025);

		if (socketReferencePtr->requireSSL)
		{
			returnvalue = SSL_read(socketReferencePtr->SSLSession, &buffer[0], 1024);
		}
		else
		{
			returnvalue = recv(socketReferencePtr->socketFd, &buffer[0], 1024, 0);
		}

		if (returnvalue < 1)
		{
			int error = socketerror;
			ioStr.clear();
			socketReferencePtr->connected = false;
			return false;
		}

		data += buffer;

		if (strstr(data.c_str(), delimeter) != 0)
		{
			std::string tempdelim = delimeter;
			strsplit(data, tempdelim, currentLine, nextLine);

			socketReferencePtr->preFetchedBufferSize = nextLine.length();

			if (socketReferencePtr->preFetchedBufferSize > 0)
			{
				socketReferencePtr->preFetchedBuffer = new unsigned char[socketReferencePtr->preFetchedBufferSize + 1];
				memset(socketReferencePtr->preFetchedBuffer, 0, socketReferencePtr->preFetchedBufferSize + 1);
				memcpy(socketReferencePtr->preFetchedBuffer, nextLine.c_str(), socketReferencePtr->preFetchedBufferSize);
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

	if (socketReferencePtr->requireSSL)
	{
		returnvalue = SSL_read(socketReferencePtr->SSLSession, &buffer[0], 1024);
	}
	else
	{
		returnvalue = recv(socketReferencePtr->socketFd, &buffer[0], 1024, 0);
	}

	if (returnvalue < 1)
	{
		int error = socketerror;
		ioStr.clear();
		socketReferencePtr->connected = false;
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

	if (socketReferencePtr->preFetchedBufferSize > 0)
	{
		if (socketReferencePtr->packet != nullptr)
		{
			delete socketReferencePtr->packet;
			socketReferencePtr->packet = nullptr;
		}
		socketReferencePtr->packet = new unsigned char[socketReferencePtr->preFetchedBufferSize];
		memcpy(socketReferencePtr->packet, socketReferencePtr->preFetchedBuffer, socketReferencePtr->preFetchedBufferSize);
		bytesleft = len - socketReferencePtr->preFetchedBufferSize;
		bufferpos = socketReferencePtr->preFetchedBufferSize;
		socketReferencePtr->preFetchedBufferSize = 0;
		delete socketReferencePtr->preFetchedBuffer;
		socketReferencePtr->preFetchedBuffer = nullptr;

		if (bytesleft < 1)
		{
			return true;
		}
	}

	while (true)
	{
		buffer = new char[bytesleft + 1];
		memset(buffer, 0, bytesleft + 1);

		if (socketReferencePtr->requireSSL)
		{
			bytesread =  SSL_read(socketReferencePtr->SSLSession, buffer, bytesleft);
		}
		else
		{
			bytesread = recv(socketReferencePtr->socketFd, buffer, bytesleft, 0);
		}

		if (bytesread < 1)
		{
			int error = socketerror;
			delete[] buffer;
			socketReferencePtr->packet = nullptr;
			len = 0;
			socketReferencePtr->connected = false;
			return false;
		}

		memcpy(socketReferencePtr->packet + bufferpos, buffer, bytesread);
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
	return socketReferencePtr->preFetchedBufferSize;
}


bool TcpClient::SendBuffer(const char* data, int &len)
{
	if (!socketReferencePtr->connected)
	{
		return false;
	}

	long sentsize = 0;

	if (socketReferencePtr->requireSSL)
	{
		sentsize = SSL_write(socketReferencePtr->SSLSession, data, len);
	}
	else
	{
		sentsize = send(socketReferencePtr->socketFd, data, len, 0);
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
	if (socketReferencePtr == nullptr)
		return false;
	return socketReferencePtr->connected;
}

unsigned long TcpClient::GetSocket()
{
	return socketReferencePtr->socketFd;
}

std::string* TcpClient::CertificateName()
{
	if (socketReferencePtr == nullptr)
		return nullptr;
	return &socketReferencePtr->certificateNamePrintable;
}
