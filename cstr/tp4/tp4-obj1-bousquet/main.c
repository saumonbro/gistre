#include <assert.h>
#include <scheduler_c/scheduler.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  Scheduler__main_out o;
  Scheduler__main_mem s;
  unsigned long cycles = 0;

  Scheduler__main_reset(&s);
  char c[2];

  for (;;) {
    Scheduler__main_step(&o, &s);
    printf("Cycle %lu\n", cycles);
    if (fgets(c, 2, stdin) == NULL) {
      exit(1);
    }
    cycles += 1;
  }
}
