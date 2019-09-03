#include "CommQueue.h"

CommQueue::CommQueue()
{
}

CommQueue::~CommQueue()
{
}

bool CommQueue::Open(const char* appname, bool create)
{
    applicationName = appname;
	mq_attr dummy;

	#if defined(_WIN32) || defined(WIN32)
	if (create)
	{
		queueReference = mq_open(applicationName.c_str(), O_CREAT | O_RDWR, 0644, &dummy);
	}
	else
	{
		queueReference = mq_open(applicationName.c_str(), O_CREAT | O_RDWR, 0644);
	}
	#else
	queueReference = mq_open(applicationName.c_str(), O_CREAT | O_RDWR, 0644);
	#endif


	if (queueReference == -1)
	{
		return false;
	}

    return true;
}

bool CommQueue::Close()
{
	if (mq_unlink(applicationName.c_str()) == -1)
	{
		return false;
	}

	if (mq_close(queueReference) == -1)
	{
		return false;
	}

    return true;
}

bool CommQueue::ReadLine(std::string &str)
{
	str.clear();

	while (true)
	{
		char buff[2] = { 0 };

		if (mq_receive(queueReference, &buff[0], 1, 0) == -1)
		{
			return false;
		}

		if (buff[0] == '\n')
		{
			break;
		}
		
		str.push_back(buff[0]);
	}

    return true;
}

bool CommQueue::WriteLine(std::string& str)
{
	if (mq_send(queueReference, str.c_str(), str.length(), 0) == -1)
	{
		return false;
	}

	return true;
}


