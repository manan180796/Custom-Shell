#include "HEAD.H"
#include "ProcessController.h"
#include "cd.h"    // for inbuilt cd command
#include "echo.h"  // for inbuilt echo command
#include "pinfo.h" // for pinfo function
#include "pwd.h"   // for inbuilt pwd command
#include "shellio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
char *HOME = (char *)malloc(PATH_SIZE);
char *PATH = (char *)malloc(PATH_SIZE);
char *pathToExecutable;

int main(int argc, char const *argv[]) {
  load();
  struct utsname buf;
  char *Input;
  char *username = (char *)malloc(100);
  char *Prefix = (char *)malloc(200);
  char *Name = (char *)malloc(100);
  char *Command;
  signal(SIGCHLD, sigchld_handler);
  uname(&buf);
  strcpy(username, "");
  sprintf(Name, "%s@%s", username, buf.nodename);
  while (1) {
    sprintf(Prefix, "%s:%s> ", Name, PATH);
    ShellOutput(Prefix);
    Input = ShellInputCommand();
    if (strlen(Input) == 0)
      continue;
    char *full = (char *)malloc(strlen(Input) + 1);
    strcpy(full, Input); //  create a separate copy of the whole input
    struct PipeLine pipeCommands;
    pipeCommands = getPipeLine(Input);
    // ShellOutput(pipeCommands.Commands[0].args[0]);
    RunPipeLine(pipeCommands);
  }

  return 0;
}
