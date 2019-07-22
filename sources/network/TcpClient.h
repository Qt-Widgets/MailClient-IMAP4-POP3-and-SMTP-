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
	bool CreateSocket(const char* servername, int serverport, bool requireSSL = false);
	bool CreateSocket(unsigned long inSocket, bool requireSSL = false);
	bool ConnectSocket(int &returncode);
	bool CloseSocket();
	bool IsConnected();
	void SetPacketDelimeter(char* str);
	void SetPacketLength(long len);

	bool SwitchToSecureMode();

	std::string* CertificateName();

	bool SendBuffer(const char* data, int &len);
	bool SendString(const std::string &str);

	bool ReceiveBuffer(int len);
	bool ReceiveString(std::string &ioStr, const char* delimeter);
	bool ReceiveString(std::string& ioStr);

	unsigned long GetSocket();

	int PendingPreFetchedBufferSize();
private:
	SocketReference*    socketReferencePtr;
};

#endif

