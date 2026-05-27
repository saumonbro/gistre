#include <assert.h>
#include <stdio.h>
#include "counter_c/counter.h"
#include "counter_c/counter_types.h"

void main()
{
  int i;

  /* Allocation des entrées */
  Counter__rcounter_out o; /* Allocation des sorties */
  Counter__rcounter_mem s;

  for (;;)
    {
      printf("Boolean (1 or 0): ");
      scanf("%d", &i); /* Lecture des entrées */
      assert(i >= 0 && i <= 1);
      Counter__rcounter_step(i, &o, &s);
      printf("Count: %d\n", o.cnt);
    }
}
