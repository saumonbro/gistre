#include "extern.h"
#include <stdio.h>

void Extern__print_fast_step(int idx, int x, int y,
                             Extern__print_fast_out *_out) {
  printf("Fast: idx=%d, x=%d, y=%d\n", idx, x, y);
}

void Extern__print_gnc_step(int idx, int y, int x,
                            Extern__print_gnc_out *_out) {
  printf("GNC:  idx=%d, y=%d, x=%d\n", idx, y, x);
}

void Extern__print_thermal_step(int idx, Extern__print_thermal_out *_out) {
  printf("Thermal: idx=%d\n", idx);
}
