#ifndef	_NETWORK
#define	_NETWORK

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
	virtual DescriptorType DescriptorType() = 0;
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

