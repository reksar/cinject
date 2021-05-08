/*
 * Test program from
 * https://xakep.ru/2015/08/12/code-injection/
 */

#include <stdio.h>

void PrintMessage(char *buffer)
{
  printf("%s", buffer);
}

int main()
{
  char *start = "start message";
  PrintMessage(start);

  char *buffer = "default message";
  while (true)
  {
    getchar();
    PrintMessage(buffer);
  }

  return 0;
}
