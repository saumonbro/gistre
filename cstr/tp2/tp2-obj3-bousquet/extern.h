#ifndef EXTERN_H
#define EXTERN_H

#include "assert.h"
#include "stdbool.h"

typedef struct {
  int value;
} Extern__read_bool_out;

typedef struct {
  int o;
}

Extern__f1_out;

typedef struct {
  int o;
}

Extern__f2_out;

typedef struct {
  int o;
}

Extern__g_out

    ;

typedef struct {
}

Extern__act_out

    ;

void Extern__f1_step(int i, Extern__f1_out *_out);

void Extern__f2_step(int i, Extern__f2_out *_out);

void Extern__g_step(Extern__g_out *_out);

void Extern__read_bool_step(int addr,

                            Extern__read_bool_out *_out);

void Extern__act_step(int addr, Extern__act_out *_out);

#endif // EXTERN_H
