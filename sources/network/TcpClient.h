#ifndef	_TCP_CLIENT
#define	_TCP_CLIENT

#include <string>
#include "Network.h"
#include "../../utils/StringEx.h"

using namespace std;

class SocketReference;

class TcpClient
{
public:
	TcpClient();
	TcpClient(int inSocket, bool requireSSL = false);

	virtual ~TcpClient();

	bool CreateSocket(const char* servername, int serverport, bool reqSSL = false);
	bool CreateSocket(unsigned long inSocket, bool requireSSL = false);
	bool ConnectSocket(int &returncode);
	bool CloseSocket();
	bool IsConnected();
	void SetPacketDelimeter(char* str);
	void SetPacketLength(long len);

	bool SwitchToSecureMode();

	bool SendBuffer(const char* data, int &len);
	bool SendString(const std::string &str);

	bool ReceiveBuffer(int len);
	bool ReceiveString(std::string &ioStr, const char* delimeter);
	bool ReceiveString(std::string& ioStr);

	unsigned long GetSocket();

	int PendingPreFetchedBufferSize();
private:
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

	const SSL_METHOD*	SSLMethod;
	SSL_CTX*			SSLContext;
	SSL*				SSLSession;
};

#endif

