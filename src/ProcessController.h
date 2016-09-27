#include "HEAD.H"
#include "shellio.h"
#include <errno.h>
#include <stdlib.h>
char **List(char *list);
void SpawnProcess(struct Command);
void RunPipeLine(struct PipeLine pipeLine) { // for foreground processes
  int n = pipeLine.n;
  pid_t *pids = (pid_t *)malloc((sizeof(pid_t) * n));
  int *prevPipe = NULL, *nextPipe = NULL;

  for (int i = 0; i < n; ++i) {
    if (i < n - 1) {
      nextPipe = (int *)malloc(sizeof(int) * 2);
      pipe(nextPipe);
    } else {
      nextPipe = NULL;
    }

    pid_t pid = fork();

    if (pid == 0) { // child process from here
      if (prevPipe != NULL) {
        dup2(prevPipe[0], STDIN_FILENO);
        close(prevPipe[1]);
      }
      if (nextPipe != NULL) {
        dup2(nextPipe[1], STDOUT_FILENO);
        close(nextPipe[0]);
      }
      SpawnProcess(pipeLine.Commands[i]);
      ShellOutputError(errno);
      exit(0);
    } // child process ends here

    pids[i] = pid;
    if (prevPipe != NULL) {
      close(prevPipe[0]);
      close(prevPipe[1]);
    }
    prevPipe = nextPipe;
  }
  for (int i = 0; i < n; ++i) {
    int status;
    waitpid(pids[i], &status, 0);
  }
}

void SpawnProcess(struct Command command) {
  if (command.in != NULL) {
    int in = open(command.in, O_RDONLY);
    if (in == -1) {
      ShellOutputError(errno);
      exit(0);
    }
    dup2(in, STDIN_FILENO);
    close(in);
  }
  if (command.out != NULL) {
    int out;
    if (command.app == 1) {
      out = open(command.out, O_WRONLY | O_CREAT | O_APPEND, 0777);
    } else {
      out = open(command.out, O_WRONLY | O_CREAT, 0777);
    }
    if (out == -1) {
      ShellOutputError(errno);
      exit(0);
    }
    dup2(out, STDOUT_FILENO);
    close(out);
  }
  execvp(command.args[0], command.args);
  ShellOutputError(errno);
}

void Background(char *Command) { // for background processes
  char **list = List(Command);
  char *cmd = list[0];
  int pipTerminal[2];
  pipe(pipTerminal);
  int pid = fork();
  if (pid == 0) {
    setpgid(0, 0);
    char Out[100];
    sprintf(Out, "%s/OutPut/%d_Output", pathToExecutable, getpid());
    int outfile = open(Out, O_WRONLY | O_APPEND | O_CREAT, 0777);
    if (outfile == -1) {
      char p[100];
      sprintf(p, "%s/OutPut", pathToExecutable);
      mkdir(p, 0777);
      outfile = open(Out, O_WRONLY | O_APPEND | O_CREAT, 0777);
    }
    dup2(outfile, STDOUT_FILENO);
    dup2(outfile, STDERR_FILENO);
    execvp(cmd, list);
    ShellOutput("No such Command found\n");
    exit(0);
  } else {
    char s[50];
    sprintf(s, "process id %d\n", pid);
    ShellOutput(s);
  }
}
