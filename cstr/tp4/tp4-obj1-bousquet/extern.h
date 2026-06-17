#ifndef EXTERN_H
#define EXTERN_H

#include "scheduler_data_c/scheduler_data.h"
#include "scheduler_data_c/scheduler_data_types.h"

typedef struct {
} Extern__deadline_miss_log_out;

typedef struct {
} Extern__print_scheduler_state_out;

void Extern__deadline_miss_log_step(int date, int task_id,
                                    Extern__deadline_miss_log_out *_out);

void Extern__print_scheduler_state_step(
    Scheduler_data__scheduler_state s, Extern__print_scheduler_state_out *_out);

#endif /* EXTERN_H */
