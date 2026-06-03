#ifndef EXTERN_H
#define EXTERN_H

#include <stdbool.h>

typedef struct {
} Extern__print_fast_out;
typedef struct {
} Extern__print_gnc_out;
typedef struct {
} Extern__print_thermal_out;

void Extern__print_fast_step(int idx, int x, int y,
                             Extern__print_fast_out *_out);
void Extern__print_gnc_step(int idx, int y, int x, Extern__print_gnc_out *_out);
void Extern__print_thermal_step(int idx, Extern__print_thermal_out *_out);

#endif /* EXTERN_H */
