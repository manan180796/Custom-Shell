#ifndef PWD_H
#define PWD_H

#include "HEAD.H"
#include "shellio.h"

void PWD() {
  char path[PATH_SIZE];
  getcwd(path, PATH_SIZE);
  strcat(path, "\n");
  ShellOutput(path);
}
#endif
