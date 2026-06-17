#include "extern.h"
#include "scheduler_data_c/scheduler_data_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Extern__deadline_miss_log_step(int date, int task_id,
                                    Extern__deadline_miss_log_out *_out) {
  printf("task %d missed deadline at date %d\n", task_id, date);
}

static const char *task_state_str(Scheduler_data__task_state state) {
  switch (state) {
  case Scheduler_data__Running:
    return "Running";
  case Scheduler_data__Ready:
    return "Ready";
  case Scheduler_data__Waiting:
    return "Waiting";
  default:
    exit(1);
  }
}

void Extern__print_scheduler_state_step(
    Scheduler_data__scheduler_state s,
    Extern__print_scheduler_state_out *_out) {
  printf("date: %d\n", s.current_date);
  for (int i = 0; i < Scheduler_data__ntasks; i++) {
    Scheduler_data__task_status t = s.tasks[i];
    printf("Task %d, Status=%s, Deadline=%d, Left=%d\n", i,
           task_state_str(t.status), t.current_deadline, t.left);
  }
}
