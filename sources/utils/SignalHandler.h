#ifndef _SIGNAL_HANDLER
#define _SIGNAL_HANDLER

// + BSD specific starts
#ifndef SIGSTKFLT
#define SIGSTKFLT 16
#endif
// + BSD specific ends

// + Windows specific starts
// Dummy signal for Windows
#ifndef ERESTART
#define ERESTART 999
#endif
// + Windows specific ends

extern int signalNumbers[];
extern const char *signalNames[];

#if !defined(WIN32) || !defined(_WIN32)
#include <siginfo.h>
#endif

class SignalCallback
{
public:
    SignalCallback(){}
    virtual void Suspend()=0;
    virtual void Resume()=0;
    virtual void Shutdown()=0;
    virtual void Alarm()=0;
    virtual void Reset()=0;
    virtual void ChildExit()=0;
    virtual void Userdefined1()=0;
    virtual void Userdefined2()=0;
};

class SignalHandler
{
public:
    SignalHandler();
    ~SignalHandler();
    void RegisterCallbackClient(SignalCallback *clientptr);
    void RegisterSignalHandlers();
private:
    static bool IsShutdownSignal(const int signum);
    void GetSignalName(const int signum);
#if defined(WIN32) || defined (_WIN32)
    static void ShutdownCallback(int signo);
    void RegisterSignals();
#else
    void RegisterSignals();
    static void AlarmCallback(int sig, siginfo_t *siginfo, void *context);
    static void SuspendCallback(int sig, siginfo_t *siginfo, void *context);
    static void ResumeCallback(int sig, siginfo_t *siginfo, void *context);
    static void ShutdownCallback(int sig, siginfo_t *siginfo, void *context);
    static void IgnoredCallback(int sig, siginfo_t *siginfo, void *context);
    static void ResetCallback(int sig, siginfo_t *siginfo, void *context);
    static void User1Callback(int sig, siginfo_t *siginfo, void *context);
    static void User2Callback(int sig, siginfo_t *siginfo, void *context);
    static void ChildExitCallback(int sig, siginfo_t *siginfo, void *context);
#endif
};

#endif
