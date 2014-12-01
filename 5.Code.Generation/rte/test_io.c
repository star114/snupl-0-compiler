#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>

void Output(int v);
void OutputHex(int v);

int Input(void);


void test_output(char *name, char *fmt, void (*f)(int))
{
  printf("testing output function '%s'\n", name);
  printf("\n");
  printf("  testing basic values\n");
  printf(fmt, 0);        fflush(stdout); f(0);
  printf(fmt, 1);        fflush(stdout); f(1);
  printf(fmt, -1);       fflush(stdout); f(-1);
  printf(fmt, INT_MAX);  fflush(stdout); f(INT_MAX);
  printf(fmt, INT_MIN);  fflush(stdout);f(INT_MIN);

  printf("\n");

  printf("  testing random numbers\n");
  srand(time(0));
  for (int i=0; i<20; i++) {
    int v = rand() - RAND_MAX/2;
    printf(fmt, v); fflush(stdout); f(v);

  }

  printf("\n");
}

void test_input(char *name, int (*f)(void))
{
  int i, v;

  printf("testing output function '%s'\n", name);
  printf("\n");

  for (i=0; i<10; i++) {
    printf("  enter input: "); fflush(stdout);
    v = Input();
    printf("  got %d\n\n", v);
  }
}


void main(int argc, char **argv)
{
  test_output("Output", "    %12d : ", &Output);
  //test_output("OutputHex", "    %8x : ", &OutputHex);
  test_input("Input", &Input);
}
