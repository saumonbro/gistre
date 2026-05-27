#include <main_c/main.h>
#include <main_c/main_types.h>

int main() {

  Main__main_out out;
  Main__main_mem self;
  Main__main_reset(&self);
  Main__main_step(&out, &self);
}
