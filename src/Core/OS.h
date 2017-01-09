#ifndef __OS
#define __OS

#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


#ifdef __APPLE__  
  #define PROCESS_EXIT(exit_code) _exit(exit_code)
#elif __linux
  #define PROCESS_EXIT(exit_code) exit(exit_code)
//TODO: Windows would be nice
#endif

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
