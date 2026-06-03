#include "main_c/main.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int i;

  Main__main_out o;
  Main__main_mem s;

  Main__main_reset(&s);

  for (;;) {
    Main__main_step(&o, &s);
    usleep(1000000);
  }
}
