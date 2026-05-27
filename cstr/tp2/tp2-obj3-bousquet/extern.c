#include "extern.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Extern__read_bool_step(int

                                addr,

                            Extern__read_bool_out *_out) {

  printf("read_bool(%d):", addr);

  fflush(stdout);

  scanf("%d", &(_out->value));
}

void Extern__act_step(int addr, Extern__act_out *_out) {}

void Extern__f1_step(int i, Extern__f1_out *_out) {}

void Extern__f2_step(int i, Extern__f2_out *_out) {

  _out->o = i

            + 5;

  printf("F2(%d)=%d\n", i, _out->o);
}

void Extern__g_step(Extern__g_out *_out) {

  static int s = 0;

  s += 7;

  _out->o = s;

  printf("G()=%d\n", _out->o);
}
