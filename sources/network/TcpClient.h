#ifndef	_TCP_CLIENT
#define	_TCP_CLIENT

#include "Network.h"
#include <string>

using namespace std;

class TcpClientAttributes;

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
	TcpClientAttributes* implStructPtr;
};

#endif

