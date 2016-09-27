#ifndef ECHO_H
#define ECHO_H

void ECHO() {
  char *str = strtok(NULL, "");
  if (str == NULL)
    str = "";
  // char *Out = (char*)malloc(strlen(str)+1);
  ShellOutput(str);
}
#endif
