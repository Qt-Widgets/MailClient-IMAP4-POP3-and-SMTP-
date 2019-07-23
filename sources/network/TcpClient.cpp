#include "TcpClient.h"

TcpClient::TcpClient()
{
	socketFd = 0;
	connected = false;
	memset((void*)&serverAddress, 0, sizeof(sockaddr_in));
	preFetchedBufferSize = 0;
	preFetchedBuffer = nullptr;
	packetSize = 0;
	packet = nullptr;
	requireSSL = false;
	phv = Undefined;

	SSLMethod = nullptr;
	SSLContext = nullptr;
	SSLSession = nullptr;
}

TcpClient::TcpClient(int inSocket, bool requireSSL)
{
	socketFd = inSocket;
	connected = true;
	memset((void*)&serverAddress, 0, sizeof(sockaddr_in));
	preFetchedBufferSize = 0;
	preFetchedBuffer = nullptr;
	packetSize = 0;
	packet = nullptr;
	requireSSL = requireSSL;
	phv = Undefined;

	//TODO - Must receive initialized SSL method, context and session
	SSLMethod = nullptr;
	SSLContext = nullptr;
	SSLSession = nullptr;
}

TcpClient::~TcpClient()
{
	if (connected)
	{
		CloseSocket();
	}

	if (preFetchedBuffer != nullptr)
	{
		delete preFetchedBuffer;
	}

	if (packet != nullptr)
	{
		delete packet;
	}
}

void TcpClient::SetPacketDelimeter(char * str)
{
	phv = Delimited;
	memcpy(packetDelimeter, str, strlen(str));
}

void TcpClient::SetPacketLength(long len)
{
	phv = FixedLength;
	packetSize = len;
}

bool TcpClient::CreateSocket(const char* servername, int serverport, bool reqSSL)
{
	serverName = servername;
	serverPort = serverport;
	requireSSL = reqSSL;

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(serverport);
	u_long nRemoteAddr;

	char ipbuffer[32] = { 0 };
	memcpy(ipbuffer, servername, 31);

	bool ip = Network::IsIP4Address(ipbuffer);

	if (!ip)
	{
		hostent* pHE = gethostbyname(serverName.c_str());
		if (pHE == 0)
		{
			nRemoteAddr = INADDR_NONE;
			return false;
		}
		nRemoteAddr = *((u_long*)pHE->h_addr_list[0]);
		serverAddress.sin_addr.s_addr = nRemoteAddr;
	}
	else
	{
		inet_pton(AF_INET, serverName.c_str(), &serverAddress.sin_addr);
	}

	if (requireSSL)
	{
		if (SSL_library_init() < 0)
		{
			return false;
		}

		SSLMethod = SSLv23_client_method();

		if ((SSLContext = SSL_CTX_new(SSLMethod)) == nullptr)
		{
			return false;
		}

		SSL_CTX_set_options(SSLContext, SSL_OP_NO_SSLv2);

		SSLSession = SSL_new(SSLContext);

		socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		SSL_set_fd(SSLSession, socketFd);
	}
	else
	{
		socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		if (socketFd == INVALID_SOCKET)
		{
			return false;
		}
	}
	return true;
}

bool TcpClient::CreateSocket(unsigned long inSocket, bool requireSSL)
{
	requireSSL = requireSSL;
	socketFd = inSocket;
	connected = true;
	return true;
}

bool TcpClient::ConnectSocket(int &returncode)
{
	if (connected == true)
	{
		return true;
	}

	returncode = connect(socketFd, (sockaddr*)&serverAddress, sizeof(sockaddr_in));

	if (returncode == SOCKET_ERROR)
	{
		returncode = socketerror;

		shutdown(socketFd, 2);
		closesocket(socketFd);
		connected = false;

		return false;
	}

	if (requireSSL)
	{
		if (SSL_connect(SSLSession) != 1)
		{
			SSL_free(SSLSession);
			shutdown(socketFd, 2);
			closesocket(socketFd);
			SSL_CTX_free(SSLContext);
			connected = false;

			return false;
		}

	}

	connected = true;
	return true;
}

bool TcpClient::SwitchToSecureMode()
{
	requireSSL = true;

	if (SSL_library_init() < 0)
	{
		return false;
	}

	SSLMethod = SSLv23_client_method();

	if ((SSLContext = SSL_CTX_new(SSLMethod)) == nullptr)
	{
		return false;
	}

	SSL_CTX_set_options(SSLContext, SSL_OP_NO_SSLv2);

	SSLSession = SSL_new(SSLContext);

	SSL_set_fd(SSLSession, socketFd);

	if (SSL_connect(SSLSession) != 1)
	{
		SSL_free(SSLSession);
		shutdown(socketFd, 2);
		closesocket(socketFd);
		SSL_CTX_free(SSLContext);
		connected = false;

		return false;
	}

	connected = true;

	return true;
}

bool TcpClient::CloseSocket()
{
	if (connected)
	{
		if (requireSSL)
		{
			SSL_shutdown(SSLSession);
			SSL_free(SSLSession);
			shutdown(socketFd, 0);
			closesocket(socketFd);
			SSL_CTX_free(SSLContext);
		}
		else
		{
			shutdown(socketFd, 0);
			closesocket(socketFd);
		}
	}
	connected = false;

	return true;
}

bool TcpClient::ReceiveString(std::string &ioStr, const char *delimeter)
{
	char	buffer[1025] = { 0 };
	long	returnvalue;
	std::string	data;
	std::string  currentLine, nextLine;

	data.clear();

	if (preFetchedBufferSize > 0)
	{
		if (strstr((char*)preFetchedBuffer, delimeter) != 0)
		{
			std::string temp = (char*)preFetchedBuffer;
			std::string tempdelim = delimeter;
			strsplit(temp, tempdelim, currentLine, nextLine);

			ioStr = currentLine;
			currentLine.clear();

			delete preFetchedBuffer;
			preFetchedBuffer = nullptr;
			preFetchedBufferSize = nextLine.length();

			if (preFetchedBufferSize > 0)
			{
				preFetchedBuffer = new unsigned char[preFetchedBufferSize + 1];
				memset(preFetchedBuffer, 0, preFetchedBufferSize + 1);
				memcpy(preFetchedBuffer, nextLine.c_str(), preFetchedBufferSize);
			}

			return true;
		}

		data = (char*)preFetchedBuffer;
		preFetchedBufferSize = 0;
		delete preFetchedBuffer;
		preFetchedBuffer = nullptr;
	}

	while (true)
	{
		memset(&buffer[0], 0, 1025);

		if (requireSSL)
		{
			returnvalue = SSL_read(SSLSession, &buffer[0], 1024);
		}
		else
		{
			returnvalue = recv(socketFd, &buffer[0], 1024, 0);
		}

		if (returnvalue < 1)
		{
			int error = socketerror;
			ioStr.clear();
			connected = false;
			return false;
		}

		data += buffer;

		if (strstr(data.c_str(), delimeter) != 0)
		{
			std::string tempdelim = delimeter;
			strsplit(data, tempdelim, currentLine, nextLine);

			preFetchedBufferSize = nextLine.length();

			if (preFetchedBufferSize > 0)
			{
				preFetchedBuffer = new unsigned char[preFetchedBufferSize + 1];
				memset(preFetchedBuffer, 0, preFetchedBufferSize + 1);
				memcpy(preFetchedBuffer, nextLine.c_str(), preFetchedBufferSize);
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

	if (requireSSL)
	{
		returnvalue = SSL_read(SSLSession, &buffer[0], 1024);
	}
	else
	{
		returnvalue = recv(socketFd, &buffer[0], 1024, 0);
	}

	if (returnvalue < 1)
	{
		int error = socketerror;
		ioStr.clear();
		connected = false;
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

	if (preFetchedBufferSize > 0)
	{
		if (packet != nullptr)
		{
			delete packet;
			packet = nullptr;
		}
		packet = new unsigned char[preFetchedBufferSize];
		memcpy(packet, preFetchedBuffer, preFetchedBufferSize);
		bytesleft = len - preFetchedBufferSize;
		bufferpos = preFetchedBufferSize;
		preFetchedBufferSize = 0;
		delete preFetchedBuffer;
		preFetchedBuffer = nullptr;

		if (bytesleft < 1)
		{
			return true;
		}
	}

	while (true)
	{
		buffer = new char[bytesleft + 1];
		memset(buffer, 0, bytesleft + 1);

		if (requireSSL)
		{
			bytesread =  SSL_read(SSLSession, buffer, bytesleft);
		}
		else
		{
			bytesread = recv(socketFd, buffer, bytesleft, 0);
		}

		if (bytesread < 1)
		{
			int error = socketerror;
			delete[] buffer;
			packet = nullptr;
			len = 0;
			connected = false;
			return false;
		}

		memcpy(packet + bufferpos, buffer, bytesread);
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
	return preFetchedBufferSize;
}


bool TcpClient::SendBuffer(const char* data, int &len)
{
	if (!connected)
	{
		return false;
	}

	long sentsize = 0;

	if (requireSSL)
	{
		sentsize = SSL_write(SSLSession, data, len);
	}
	else
	{
		sentsize = send(socketFd, data, len, 0);
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
	return connected;
}

unsigned long TcpClient::GetSocket()
{
	return socketFd;
}
