#include <memory>
#include "cmdline_options.hpp"
#include "config.h"
#include "config.hpp"
#include "scheduler.hpp"

int main(int argc, char* argv[]) {
  auto cmd_options = std::make_unique<CmdlineOptions>(argc, argv);

  auto scheduler = std::make_unique<Scheduler>();

  return 0;
}