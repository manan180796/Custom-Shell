#ifndef HEAD_H
#define HEAD_H

#include "shellio.h"
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PATH_SIZE 200
extern char *HOME;
extern char *PATH;
extern char *pathToExecutable;

struct Command {
  char **args;
  char *out, *in;
  int app;
};

struct PipeLine {
  struct Command *Commands;
  int n;
};

struct p_stat {
  int pid;
  char comm[50];
  char state;
  int ppid;
  int pgrp;
  int session;
  int tty_nr;
  int tpgid;
  unsigned int flags;
  unsigned long int minflt, cminflt, majflt, cmajflt;
  unsigned long int utime, stime;
  long int cutime, cstime;
  long int priority;
  long int nice;
  long int num_threads;
  long int itrealvalue;
  unsigned long long starttime;
  unsigned long int vsize;
  long int rss;
  unsigned long int rsslim;
  unsigned long int startcode, endcode;
  unsigned long int startstack;
  unsigned long int kstkesp, kstkeip;
  unsigned long int signals, blocked, sigignore, sigcatch;
  unsigned long int wchan;
  unsigned long int nswap, cnswap;
  int exit_signal;
  int processor;
  unsigned int rt_priority;
  unsigned int policy;
  unsigned long long delayacct_blkio_ticks;
  unsigned long int guest_time;
  long int cguest_time;
  unsigned long int start_data, end_data;
  unsigned long int start_brk;
  unsigned long int arg_start, arg_end;
  unsigned long int env_start, env_end;
  int exit_code;
  char execpath[200];
};

void sigproc(int);
void resolvepath(char *path);
void pstat(pid_t pid, struct p_stat *pstatus);
void load();
void UpdatePath();

void load() {
  getcwd(HOME, PATH_SIZE);
  UpdatePath();
  struct p_stat p;
  pstat(getpid(), &p);
  pathToExecutable = (char *)malloc(strlen(p.execpath) + 1);
  strcpy(pathToExecutable, p.execpath);
  int length = strlen(pathToExecutable) - 1;
  while (length >= 0 && pathToExecutable[length] != '/') {
    pathToExecutable[length] = 0;
    length--;
  }
  if (length >= 0) {
    pathToExecutable[length] = 0;
  }
}
void UpdatePath() {
  getcwd(PATH, PATH_SIZE);
  resolvepath(PATH);
}
char **List(char *list) {
  char **LIST = (char **)malloc(sizeof(char *) * 20);
  int len = 0;
  char *arg = strtok(list, " ");
  do {
    LIST[len] = (char *)malloc(100);
    strcpy(LIST[len], arg);
    len++;
    arg = strtok(NULL, " ");
    // ShellOutput(arg);
  } while (arg != NULL && arg[0] != 0);
  LIST[len] = NULL;
  return LIST;
}

struct Command getCommand(char *input) {
  char **list = List(input);
  char *out = NULL, *in = NULL;
  int app = 0;
  int i = 0;
  int found = 0;
  while (true) {
    if (list[i] == NULL)
      break;
    if (!found) {
      found = (strcmp(">>", list[i]) == 0);
      if (found) {
        out = list[i + 1];
        app = 1;
      }
    }
    if (found) {
      list[i] = list[i + 2];
    }
    if (list[i] == NULL)
      break;
    ++i;
  }
  found = 0;
  i = 0;
  if (out == NULL) {
    while (true) {
      if (list[i] == NULL)
        break;
      if (!found) {
        found = (strcmp(">", list[i]) == 0);
        if (found) {
          out = list[i + 1];
          app = 0;
        }
      }
      if (found) {
        list[i] = list[i + 2];
      }
      if (list[i] == NULL)
        break;
      ++i;
    }
  }
  found = 0;
  i = 0;
  while (true) {
    if (list[i] == NULL)
      break;
    if (!found) {
      found = (strcmp("<", list[i]) == 0);
      if (found) {
        in = list[i + 1];
      }
    }
    if (found) {
      list[i] = list[i + 2];
    }
    if (list[i] == NULL)
      break;
    ++i;
  }
  struct Command commmand;
  commmand.args = list;
  commmand.in = in;
  commmand.out = out;
  commmand.app = app;
  return commmand;
}

void sigchld_handler(int signum) {
  pid_t pid;
  int status;
  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
    char MSG[20];
    // kill(pid,SIGKILL);
    char Out[100];
    sprintf(Out, "%s/OutPut/%d_Output", pathToExecutable, pid);
    if (status == 0) {
      int output = open(Out, O_RDONLY);
      char c;
      while (read(output, &c, 1) > 0) {
        write(STDOUT_FILENO, &c, 1);
      }
    }
    // int outfile = open(Out,O_WRONLY|O_APPEND|O_CREAT,0777);
    unlink(Out);
    sprintf(MSG, "process %d has ended with status %d \n", pid, status);
    ShellOutput(MSG);
  }
}
void resolvepath(char *path) {
  if (strncmp(HOME, path, strlen(HOME)) == 0) {
    int j = strlen(HOME);
    path[0] = '~';
    int i = 1;
    do {
      path[i] = path[j];
      ++i;
      ++j;
    } while (path[j - 1] != '\0');
  }
}

struct PipeLine getPipeLine(char *input) {
  int len = strlen(input);
  struct PipeLine pipeLine;
  pipeLine.n = 1;
  for (int i = 0; i < len; ++i) {
    pipeLine.n += (input[i] == '|');
  }
  pipeLine.Commands =
      (struct Command *)malloc(sizeof(struct Command) * pipeLine.n);
  char COM[100];
  for (int i = 0; i < pipeLine.n; ++i) {
    char *cmd = strtok(input, "|");
    int n = strlen(cmd);
    input += n + 1;
    pipeLine.Commands[i] = getCommand(cmd);
  }
  return pipeLine;
}

void pstat(pid_t pid, struct p_stat *pstatus) {
  char ProcStatPath[50];
  sprintf(ProcStatPath, "/proc/%d/stat", pid);
  char procData[500];
  int procstatFile = open(ProcStatPath, O_RDONLY);
  int procDataLength = read(procstatFile, procData, 500);
  procData[procDataLength] = 0;
  sscanf(procData, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld "
                   "%ld %ld %ld %ld %llu %lu %ld %lu %lu %lu %lu %lu %lu %lu "
                   "%lu %lu %lu %lu %lu %lu %d %d %u %u %llu %lu %ld %lu %lu "
                   "%lu %lu %lu %lu %lu %d",
         &(pstatus->pid), (pstatus->comm), &(pstatus->state), &(pstatus->ppid),
         &(pstatus->pgrp), &(pstatus->session), &(pstatus->tty_nr),
         &(pstatus->tpgid), &(pstatus->flags), &(pstatus->minflt),
         &(pstatus->cminflt), &(pstatus->majflt), &(pstatus->cmajflt),
         &(pstatus->utime), &(pstatus->stime), &(pstatus->cutime),
         &(pstatus->cstime), &(pstatus->priority), &(pstatus->nice),
         &(pstatus->num_threads), &(pstatus->itrealvalue),
         &(pstatus->starttime), &(pstatus->vsize), &(pstatus->rss),
         &(pstatus->rsslim), &(pstatus->startcode), &(pstatus->endcode),
         &(pstatus->startstack), &(pstatus->kstkesp), &(pstatus->kstkeip),
         &(pstatus->signals), &(pstatus->blocked), &(pstatus->sigignore),
         &(pstatus->sigcatch), &(pstatus->wchan), &(pstatus->nswap),
         &(pstatus->cnswap), &(pstatus->exit_signal), &(pstatus->processor),
         &(pstatus->rt_priority), &(pstatus->policy),
         &(pstatus->delayacct_blkio_ticks), &(pstatus->guest_time),
         &(pstatus->cguest_time), &(pstatus->start_data), &(pstatus->end_data),
         &(pstatus->start_brk), &(pstatus->arg_start), &(pstatus->arg_end),
         &(pstatus->env_start), &(pstatus->env_end), &(pstatus->exit_code));
  close(procstatFile);
  char procexecpath[25];
  sprintf(procexecpath, "/proc/%d/exe", pid);
  int length = readlink(procexecpath, pstatus->execpath, 200);
  pstatus->execpath[length] = 0;
  // resolvepath(pstatus->execpath);
}

#endif
