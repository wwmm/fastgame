#pragma once

#include <adwaita.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include "util.hpp"

namespace ui::cpu {

G_BEGIN_DECLS

#define FG_TYPE_CPU (cpu_get_type())

G_DECLARE_FINAL_TYPE(Cpu, cpu, FG, CPU, GtkBox)

G_END_DECLS

auto create() -> Cpu*;

auto get_enable_batch_scheduler(Cpu* self) -> bool;

void set_enable_batch_scheduler(Cpu* self, const bool& state);

auto get_child_runs_first(Cpu* self) -> bool;

void set_child_runs_first(Cpu* self, const bool& state);

auto get_use_cpu_dma_latency(Cpu* self) -> bool;

void set_use_cpu_dma_latency(Cpu* self, const bool& state);

auto get_use_realtime_wineserver(Cpu* self) -> bool;

void set_use_realtime_wineserver(Cpu* self, const bool& state);

auto get_niceness(Cpu* self) -> int;

void set_niceness(Cpu* self, const int& value);

auto get_frequency_governor(Cpu* self) -> std::string;

void set_frequency_governor(Cpu* self, const std::string& name);

void set_game_cores(Cpu* self, const std::vector<std::string>& list);

void set_wineserver_cores(Cpu* self, const std::vector<std::string>& list);

//   auto get_game_cores() -> std::vector<std::string>;

//   auto get_wineserver_cores() -> std::vector<std::string>;

}  // namespace ui::cpu
