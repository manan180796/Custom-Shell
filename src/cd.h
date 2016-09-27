#ifndef CD_H
#define CD_H

void CD();
void CD() {
  char *DIR =
      strtok(NULL, " "); //  extract the remaining string after trimming 'cd'
  int result;
  if (DIR != NULL &&
      strlen(DIR) > 0) { //  check if any directory is given or not
    result = chdir(DIR); //  if yes then change to that directory
  } else {
    result = chdir(HOME); //  else change to home directory
  }
  if (result == -1) { //  check for any error
    int eno = errno;
    ShellOutputError(eno); //  if yes output the error
  }
  UpdatePath(); //  update the path
}
#endif
