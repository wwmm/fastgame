#include "cpu.hpp"
#include <glibmm/i18n.h>
#include <gtkmm/checkbutton.h>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include "util.hpp"

namespace fs = std::filesystem;

Cpu::Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
  // loading glade widgets

  builder->get_widget("use_sched_batch", use_sched_batch);
  builder->get_widget("child_runs_first", child_runs_first);
  builder->get_widget("frequency_governor", frequency_governor);
  builder->get_widget("affinity_flowbox", affinity_flowbox);
  builder->get_widget("use_cpu_dma_latency", use_cpu_dma_latency);
  builder->get_widget("realtime_wineserver", realtime_wineserver);

  niceness = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("niceness"));

  // initializing widgets

  n_cores = std::thread::hardware_concurrency();

  util::debug(log_tag + "number of cpu cores: " + std::to_string(n_cores));

  for (uint n = 0; n < n_cores; n++) {
    auto* checkbutton = Gtk::make_managed<Gtk::CheckButton>(std::to_string(n));

    checkbutton->set_active(true);

    affinity_flowbox->add(*checkbutton);
  }

  // We assume that all cores are set to the same frequency governor and that the system has at least one core. In this
  // case reading the core 0 property should be enough

  auto list_governors = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");

  auto selected_governor = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")[0];

  for (auto& value : list_governors) {
    if (value.empty()) {
      continue;
    }

    frequency_governor->append(value);
  }

  frequency_governor->set_active_text(selected_governor);

  child_runs_first->set_active(
      static_cast<bool>(std::stoi(util::read_system_setting("/proc/sys/kernel/sched_child_runs_first")[0])));
}

Cpu::~Cpu() {
  util::debug(log_tag + "destroyed");
}

auto Cpu::add_to_stack(Gtk::Stack* stack) -> Cpu* {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/cpu.glade");

  Cpu* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui);

  stack->add(*ui, "cpu", _("CPU"));

  return ui;
}

auto Cpu::get_enable_batch_scheduler() -> bool {
  return use_sched_batch->get_active();
}

void Cpu::set_enable_batch_scheduler(const bool& state) {
  use_sched_batch->set_active(state);
}

auto Cpu::get_child_runs_first() -> bool {
  return child_runs_first->get_active();
}

void Cpu::set_child_runs_first(const bool& state) {
  child_runs_first->set_active(state);
}

auto Cpu::get_frequency_governor() -> std::string {
  return frequency_governor->get_active_text();
}

void Cpu::set_frequency_governor(const std::string& name) {
  frequency_governor->set_active_text(name);
}

auto Cpu::get_cores() -> std::vector<std::string> {
  std::vector<std::string> list;

  auto children = affinity_flowbox->get_children();

  for (auto* child : children) {
    auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

    auto* checkbutton = dynamic_cast<Gtk::CheckButton*>(flowbox_child->get_child());

    if (checkbutton->get_active()) {
      list.emplace_back(checkbutton->get_label());
    }
  }

  return list;
}

void Cpu::set_cores(const std::vector<std::string>& list) {
  auto children = affinity_flowbox->get_children();

  for (auto* child : children) {
    auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

    auto* checkbutton = dynamic_cast<Gtk::CheckButton*>(flowbox_child->get_child());

    if (std::find(list.begin(), list.end(), checkbutton->get_label()) != list.end()) {
      checkbutton->set_active(true);
    } else {
      checkbutton->set_active(false);
    }
  }
}

auto Cpu::get_use_cpu_dma_latency() -> bool {
  return use_cpu_dma_latency->get_active();
}

void Cpu::set_use_cpu_dma_latency(const bool& state) {
  use_cpu_dma_latency->set_active(state);
}

auto Cpu::get_use_realtime_wineserver() -> bool {
  return realtime_wineserver->get_active();
}

void Cpu::set_use_realtime_wineserver(const bool& state) {
  realtime_wineserver->set_active(state);
}

auto Cpu::get_niceness() -> int {
  return static_cast<int>(niceness->get_value());
}

void Cpu::set_niceness(const int& value) {
  niceness->set_value(value);
}