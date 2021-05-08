// https://xakep.ru/2015/08/12/code-injection/

#include <stdio.h>

void PrintMessage(char *buffer)
{
  printf("%s", buffer);
}

void main()
{
  char *buffer = "default message";

  while (true)
  {
    getchar();
    PrintMessage(buffer);
  }
}
