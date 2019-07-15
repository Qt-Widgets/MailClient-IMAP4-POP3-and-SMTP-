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
	virtual DescriptorType descriptorType() = 0;
};


class Network
{
public:
	Network();
	virtual ~Network();
	static bool isIPAddress(char* str);
	static bool isIP6Address(char* str);
	static bool isIP4Address(char* str);
	static void getLocalHostName(char *hostname);
	static void getLocalIPAddress(const int newServerfd, char *ipaddress);
	static void getEndPoint(const char* url, char* host, int &port);
	static bool addToDescriptors(void* sockref);
	static bool removeFromDescriptors(void* sockref);
	static Descriptor* getActiveDescriptor();
private:
};

#endif

