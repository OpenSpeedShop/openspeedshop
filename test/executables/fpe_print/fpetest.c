#include <stdio.h>
#include <values.h>

int main(void)
{
    unsigned int large_int = ULONG_MAX;
    double a;
    double b;

    /* divide by zero */
    {
      a = 1.0; b = time(0) - time(0);
      printf("DOUBLE DIVIDE BY ZERO, 1.0/0.0\n");
      a /= b;
      printf("%f\n", a);
    }

    /* integer divide by zero */
    {
      a = 1.0; b = time(0) - time(0);
      printf("INTEGER DIVIDE BY ZERO, ULONG_MAX/0.0\n");
      large_int /= 0.0;
      printf("%f\n", large_int);
    }

    /* overflow */
    {
      a = DBL_MAX; b = DBL_MAX;
      printf("OVERFLOW on DBL_MAX+DBL_MAX\n");
      a += b;
      printf("%f\n", a);
    }

    /* underflow */
    {
      a = DBL_MIN; b = 2;
      printf("UNDERFLOW on DBL_MIN/2\n");
      a /= b;
      printf("%f\n", a);

    }

    /* inexact result */
    {
      a = DBL_MIN; b = DBL_MAX;
      printf("UNDERFLOW on DBL_MIN/DBL_MAX\n");
      a /= b;
      printf("%f\n", a);
    }

    fprintf(stderr, "DONE!\n");
    return 0;
}
