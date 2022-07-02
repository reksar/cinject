/*
 * Test program from
 * https://xakep.ru/2015/08/12/code-injection
 */

#include <stdio.h>

void PrintMessage(const char *buffer)
{
  printf("%s", buffer);
}

int main()
{
  const char start[] = "start message";
  PrintMessage(start);

  const char buffer[] = "default message";
  while (1)
  {
    getchar();
    PrintMessage(buffer);
  }

  return 0;
}
