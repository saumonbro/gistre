#ifndef EXTERN_TYPES_H
#define EXTERN_TYPES_H

#include "assert.h"
#include "pervasives.h"
#include "stdbool.h"

extern const int Extern__ntasks;
/*
typedef enum {
  Extern__Running,
  Extern__Ready,
  Extern__Waiting
} Extern__task_state;

typedef struct Extern__task_status {
  Extern__task_state status;
  int current_deadline;
  int left;
} Extern__task_status;

typedef struct Extern__scheduler_state {
  int current_date;
  Extern__task_status tasks[2];
} Extern__scheduler_state;*/

#endif /* ! EXTERN_TYPES_H */
