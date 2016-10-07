#include "OS.h"

ProcessHandle ProcessHandle::INVALID = ProcessHandle(-1);

ProcessHandle::ProcessHandle(){}

ProcessHandle::ProcessHandle(pid_t pid)
{
  mPid = pid;

  // TODO: If pid == -1, mark myself as invalid (?)
}

ProcessHandle::~ProcessHandle(){}

ProcessHandle ProcessHandle::SpawnProcess(const std::string& programPath)
{
    pid_t pid = fork(); /* Create a child process */
    
    switch (pid)
    {
        case -1: /* Error */
	    return ProcessHandle::INVALID;

        case 0: /* Child process */
            execl(programPath.c_str(), NULL); /* Execute the program */
	    // TODO: excel doesn't return, if we get here something got eFFed.
            exit(1);

        default: /* Parent process */
            return ProcessHandle(pid);
    }
}

int ProcessHandle::WaitForExit() 
{
    int status;
    while (!WIFEXITED(status))
    {
        waitpid(mPid, &status, 0); /* Wait for the process to complete */
    }
    mExitStatus = status;
    return status;
}
