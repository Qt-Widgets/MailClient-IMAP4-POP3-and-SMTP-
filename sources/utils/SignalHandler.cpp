#include "SignalHandler.h"
#include "Logger.h"
#include <signal.h>
#include <memory.h>
#include <string.h>

#if !defined(WIN32) || !defined(_WIN32)
int signalNumbers[] = {SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGTRAP, SIGABRT, SIGBUS, SIGFPE, SIGSEGV, SIGPIPE, SIGTERM, SIGSTKFLT, SIGUSR1, SIGUSR2, SIGCHLD};
const char *signalNames[] = {"SIGHUP", "SIGINT", "SIGQUIT", "SIGILL", "SIGTRAP", "SIGABRT", "SIGBUS", "SIGFPE", "SIGSEGV", "SIGPIPE", "SIGTERM", "SIGSTKFLT", "SIGUSR1", "SIGUSR2", "SIGCHLD"};
#else
int signalNumbers[] = {SIGINT, SIGILL, SIGABRT, SIGFPE, SIGSEGV, SIGTERM};
const char *signalNames[] = {"SIGINT", "SIGILL", "SIGABRT", "SIGFPE", "SIGSEGV", "SIGTERM"};
#endif

char signalNameString[16]={0};

SignalCallback *callback = NULL;


SignalHandler::SignalHandler()
{
}

SignalHandler::~SignalHandler()
{
}

void SignalHandler::RegisterCallbackClient(SignalCallback *clientptr)
{
    callback = clientptr;
}

void SignalHandler::RegisterSignalHandlers()
{
    RegisterSignals();
}

#if defined(WIN32) || defined (_WIN32)
void SignalHandler::ShutdownCallback(int signo)
{
    if(callback && signo != SIGSEGV)
    {
        callback->Shutdown();
    }
}
#endif

void SignalHandler::GetSignalName(const int signum)
{
    int ctr = 0;

    memset((char*)&signalNameString[0], 0, sizeof(signalNameString));
    strcpy(signalNameString, "<Not Named>");

    #if !defined(WIN32) || !defined(_WIN32)
        for(ctr = 0; ctr < 15; ctr++)
        {
            if(signalNumbers[ctr] == signum)
            {
                memset((char*)&signalNameString[0], 0, sizeof(signalNameString));
                strcpy(signalNameString, signalNames[ctr]);
                break;
            }
        }
    #else
        for(ctr = 0; ctr < 6; ctr++)
        {
            if(signalNumbers[ctr] == signum)
            {
                memset((char*)&signalNameString[0], 0, sizeof(signalNameString));
                strcpy(signalNameString, signalNames[ctr]);
                break;
            }
        }
    #endif
}

bool SignalHandler::IsShutdownSignal(const int signum)
{
    int ctr = 0;

    bool found = false;

    #if !defined(WIN32) || !defined(_WIN32)
        for(ctr = 0; ctr < 15; ctr++)
        {
            if(signalNumbers[ctr] == signum)
            {
                found = true;
                break;
            }
        }
    #else
        for(ctr = 0; ctr < 6; ctr++)
        {
            if(signalNumbers[ctr] == signum)
            {
                found = true;
                break;
            }
        }
    #endif

    return found;
}

#if defined(WIN32) || defined(_WIN32)
void SignalHandler::RegisterSignals()
{
    if(signal(SIGINT, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }

    if(signal(SIGILL, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }

    if(signal(SIGABRT, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }

    if(signal(SIGFPE, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }

    if(signal(SIGSEGV, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }

    if(signal(SIGTERM, SignalHandler::ShutdownCallback) == SIG_ERR)
    {
    }
}
#else
void SignalHandler::RegisterSignals()
{
    struct sigaction act;

    for(int signum = 1; signum < 32; signum++)
    {
        getSignalName(signum);

        memset (&act, '\0', sizeof(act));
        act.sa_flags = SA_SIGINFO;

        switch(signum)
        {
            case SIGKILL:
            case SIGSTOP:
            {
                continue;
            }
            case SIGINT:
            case SIGQUIT:
            case SIGILL:
            case SIGTRAP:
            case SIGABRT:
            case SIGBUS:
            case SIGFPE:
            case SIGSEGV:
            case SIGPIPE:
            case SIGTERM:
            case SIGSTKFLT:
            {
                act.sa_sigaction = &SignalHandler::ShutdownCallback;
                break;
            }
            case SIGALRM:
            {
                act.sa_sigaction = &SignalHandler::AlarmCallback;
                break;
            }
            case SIGTSTP:
            {
                act.sa_sigaction = &SignalHandler::SuspendCallback;
                break;
            }
            case SIGCONT:
            {
                act.sa_sigaction = &SignalHandler::ResumeCallback;
                break;
            }
            case SIGHUP:
            {
                act.sa_sigaction = &SignalHandler::ResetCallback;
                break;
            }
            case SIGCHLD:
            {
                act.sa_sigaction = &SignalHandler::ChildExitCallback;
                break;
            }
            case SIGUSR1:
            {
                act.sa_sigaction = &SignalHandler::User1Callback;
                break;
            }
            case SIGUSR2:
            {
                act.sa_sigaction = &SignalHandler::User2Callback;
                break;
            }
            default:
            {
                act.sa_sigaction = &SignalHandler::IgnoredCallback;
            }
        }

        if (sigaction(signum, &act, NULL) < 0)
        {
        }
        else
        {
        }
    }
}

void SignalHandler::SuspendCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->suspend();
    }
}

void SignalHandler::ResumeCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->resume();
    }
}

void SignalHandler::ShutdownCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(sig == SIGSEGV)
    {
        exit(1);
    }

    if(callback)
    {
        callback->shutdown();
    }
}

void SignalHandler::IgnoredCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(SignalHandler::isShutdownSignal(sig))
    {
        ShutdownCallback(sig, siginfo, context);
        return;
    }
}

void SignalHandler::AlarmCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->alarm();
    }
}

void SignalHandler::ResetCallback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->reset();
    }
}

void SignalHandler::ChildExitCallback(int sig, siginfo_t *siginfo, void *context)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
    {
    }

    if(callback)
    {
        callback->childExit();
    }
}

void SignalHandler::User1Callback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->userdefined1();
    }
}

void SignalHandler::User2Callback(int sig, siginfo_t *siginfo, void *context)
{
    if(callback)
    {
        callback->userdefined2();
    }
}



#endif
