#include "cpu.hpp"
#include <glibmm/i18n.h>
#include <gtkmm/checkbutton.h>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include "util.hpp"

namespace fs = std::filesystem;

Cpu::Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder, Application* application)
    : Gtk::Grid(cobject), app(application) {
  // loading glade widgets

  builder->get_widget("use_sched_batch", use_sched_batch);
  builder->get_widget("child_runs_first", child_runs_first);
  builder->get_widget("frequency_governor", frequency_governor);
  builder->get_widget("affinity_flowbox", affinity_flowbox);

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
    frequency_governor->append(value);
  }

  frequency_governor->set_active_text(selected_governor);

  child_runs_first->set_active(
      static_cast<bool>(std::stoi(util::read_system_setting("/proc/sys/kernel/sched_child_runs_first")[0])));
}

Cpu::~Cpu() {
  util::debug(log_tag + "destroyed");
}

void Cpu::add_to_stack(Gtk::Stack* stack, Application* app) {
  auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/cpu.glade");

  Cpu* ui = nullptr;

  builder->get_widget_derived("widgets_grid", ui, app);

  stack->add(*ui, "cpu", _("CPU"));
}
