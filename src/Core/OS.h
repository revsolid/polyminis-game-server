#ifndef __OS
#define __OS

#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

class ProcessHandle
{
public:
  static ProcessHandle SpawnProcess(const std::string& path);
  static ProcessHandle INVALID;

  int WaitForExit();

  ProcessHandle();
  ProcessHandle(pid_t pid);
  virtual ~ProcessHandle();

private:
  pid_t mPid;
  int mExitStatus;
};
#endif
