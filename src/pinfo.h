#ifndef PINFO_H
#define PINFO_H
#include "HEAD.H"
#include "errno.h"
#include "shellio.h"

void PINFO() {
  pid_t pid;
  char *PIDString = strtok(NULL, " ");
  if (PIDString == NULL || strlen(PIDString) <= 0) {
    pid = getpid();
  } else {
    sscanf(PIDString, "%d", &pid);
  }
  struct p_stat procstat;
  pstat(pid, &procstat);
  char Output[50];
  sprintf(Output, "pid = %d\n", procstat.pid);
  ShellOutput(Output);
  sprintf(Output, "Command = %s\n", procstat.comm);
  ShellOutput(Output);
  sprintf(Output, "process status = %c\n", procstat.state);
  ShellOutput(Output);
  sprintf(Output, "Virtual Memory = %lu\n", procstat.vsize);
  ShellOutput(Output);
  sprintf(Output, "Path = %s\n", procstat.execpath);
  ShellOutput(Output);
}
#endif
