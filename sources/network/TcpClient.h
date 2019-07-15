#ifndef	_TCP_CLIENT
#define	_TCP_CLIENT

#include <string>
#include "Network.h"

using namespace std;

class SocketReference;

class TcpClient
{
public:
	TcpClient();
	TcpClient(int inSocket, bool requireSSL = false);
	TcpClient(const TcpClient& other);
	TcpClient& operator=(const TcpClient& other);

	virtual ~TcpClient();
	bool createSocket(const char* servername, int serverport, bool requireSSL = false);
	bool createSocket(unsigned long inSocket, bool requireSSL = false);
	bool connectSocket(int &returncode);
	bool closeSocket();
	bool isConnected();
	void setPacketDelimeter(char* str);
	void setPacketLength(long len);

	void switchToSecureMode();

	std::string* certificateName();

	bool sendBuffer(const char* data, int &len);
	bool sendString(const std::string &str);

	bool receiveBuffer(int len);
	bool receiveString(std::string &ioStr, const char* delimeter);

	unsigned long getSocket();

	int pendingPreFetchedBufferSize();
private:
	SocketReference*    _SocketReference;
};

#endif

