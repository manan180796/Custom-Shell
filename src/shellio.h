#ifndef SHELLIO_H
#define SHELLIO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
char *ShellInputCommand();
int ShellOutput(char *);
int ShellOutputError(int);

char *ShellInputCommand() { //	to take input from terminal
  char input;
  char *InputBuffer = (char *)malloc(100);
  int BufferLength = 0;
  InputBuffer[BufferLength] = '\0';
  while (read(STDIN_FILENO, &input, 1) > 0) {
    if (input != '\n') {
      InputBuffer[BufferLength] = input;
      ++BufferLength;
      InputBuffer[BufferLength] = '\0';
    } else {
      break;
    }
  }
  while (*InputBuffer != 0 && *InputBuffer == ' ')
    ++InputBuffer;
  int n = strlen(InputBuffer);
  while (n > 0 && InputBuffer[n - 1] == ' ')
    InputBuffer[n--] = 0;
  return InputBuffer;
}

int ShellOutput(char *Output) { //	to show output
  if (Output == NULL)
    return 0;
  char *Out = (char *)malloc(strlen(Output) + 2);
  strcpy(Out, Output);
  return write(STDOUT_FILENO, Out, strlen(Out));
}

int ShellOutputError(int error) { // to show error according to errno
  char *ERror = strerror(error);
  char *Error = (char *)malloc(strlen(ERror) + 2);
  strcpy(Error, ERror);
  strcat(Error, "\n");
  return write(STDERR_FILENO, Error, strlen(Error));
}
#endif
