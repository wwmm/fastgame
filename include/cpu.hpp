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

//   auto get_enable_batch_scheduler() -> bool;

//   void set_enable_batch_scheduler(const bool& state);

//   auto get_child_runs_first() -> bool;

//   void set_child_runs_first(const bool& state);

//   auto get_frequency_governor() -> std::string;

//   void set_frequency_governor(const std::string& name);

//   auto get_use_cpu_dma_latency() -> bool;

//   void set_use_cpu_dma_latency(const bool& state);

//   auto get_use_realtime_wineserver() -> bool;

//   void set_use_realtime_wineserver(const bool& state);

//   auto get_niceness() -> int;

//   void set_niceness(const int& value);

//   auto get_game_cores() -> std::vector<std::string>;

//   void set_game_cores(const std::vector<std::string>& list);

//   auto get_workqueue_cores() -> std::vector<std::string>;

//   void set_workqueue_cores(const std::vector<std::string>& list);

//   auto get_wineserver_cores() -> std::vector<std::string>;

//   void set_wineserver_cores(const std::vector<std::string>& list);

}  // namespace ui::cpu

// class Cpu : public Gtk::Grid {
//  public:
//   Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
//   Cpu(const Cpu&) = delete;
//   auto operator=(const Cpu&) -> Cpu& = delete;
//   Cpu(const Cpu&&) = delete;
//   auto operator=(const Cpu&&) -> Cpu& = delete;
//   ~Cpu() override;

//   static auto add_to_stack(Gtk::Stack* stack) -> Cpu*;

//  private:
//   std::string log_tag = "cpu: ";

//   uint n_cores = 1;

//   Gtk::Switch *use_sched_batch = nullptr, *child_runs_first = nullptr, *use_cpu_dma_latency = nullptr,
//               *realtime_wineserver = nullptr;

//   Gtk::ComboBoxText* frequency_governor = nullptr;

//   Gtk::FlowBox *game_affinity_flowbox = nullptr, *workqueue_affinity_flowbox = nullptr,
//                *wineserver_affinity_flowbox = nullptr;

//   Glib::RefPtr<Gtk::Adjustment> niceness;

//   static auto get_cores(Gtk::FlowBox* flowbox) -> std::vector<std::string>;

//   static void set_cores(Gtk::FlowBox* flowbox, const std::vector<std::string>& list);
// };
