#include "cpu.hpp"

namespace ui::cpu {

using namespace std::string_literals;

auto constexpr log_tag = "cpu: ";

GListStore* model;

struct _Cpu {
  GtkPopover parent_instance;

  GtkColumnView* columnview;
};

G_DEFINE_TYPE(Cpu, cpu, GTK_TYPE_BOX)

void dispose(GObject* object) {
  util::debug(log_tag + "disposed"s);

  G_OBJECT_CLASS(cpu_parent_class)->dispose(object);
}

void finalize(GObject* object) {
  util::debug(log_tag + "finalized"s);

  G_OBJECT_CLASS(cpu_parent_class)->finalize(object);
}

void cpu_class_init(CpuClass* klass) {
  auto* object_class = G_OBJECT_CLASS(klass);
  auto* widget_class = GTK_WIDGET_CLASS(klass);

  object_class->dispose = dispose;
  object_class->finalize = finalize;

  gtk_widget_class_set_template_from_resource(widget_class, "/com/github/wwmm/fastgame/ui/cpu.ui");

  // gtk_widget_class_bind_template_child(widget_class, Cpu, columnview);

  // gtk_widget_class_bind_template_callback(widget_class, on_add_line);
  // gtk_widget_class_bind_template_callback(widget_class, on_remove_line);
  // gtk_widget_class_bind_template_callback(widget_class, on_name_changed);
  // gtk_widget_class_bind_template_callback(widget_class, on_value_changed);
}

void cpu_init(Cpu* self) {
  gtk_widget_init_template(GTK_WIDGET(self));
}

auto create() -> Cpu* {
  return static_cast<Cpu*>(g_object_new(FG_TYPE_CPU, nullptr));
}

}  // namespace ui::cpu

// namespace fs = std::filesystem;

// Cpu::Cpu(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder) : Gtk::Grid(cobject) {
//   // loading glade widgets

//   builder->get_widget("use_sched_batch", use_sched_batch);
//   builder->get_widget("child_runs_first", child_runs_first);
//   builder->get_widget("frequency_governor", frequency_governor);
//   builder->get_widget("game_affinity_flowbox", game_affinity_flowbox);
//   builder->get_widget("workqueue_affinity_flowbox", workqueue_affinity_flowbox);
//   builder->get_widget("use_cpu_dma_latency", use_cpu_dma_latency);
//   builder->get_widget("realtime_wineserver", realtime_wineserver);
//   builder->get_widget("wineserver_affinity_flowbox", wineserver_affinity_flowbox);

//   niceness = Glib::RefPtr<Gtk::Adjustment>::cast_dynamic(builder->get_object("niceness"));

//   // initializing widgets

//   n_cores = std::thread::hardware_concurrency();

//   util::debug(log_tag + "number of cpu cores: " + std::to_string(n_cores));

//   for (uint n = 0; n < n_cores; n++) {
//     auto* checkbutton1 = Gtk::make_managed<Gtk::CheckButton>(std::to_string(n));
//     auto* checkbutton2 = Gtk::make_managed<Gtk::CheckButton>(std::to_string(n));
//     auto* checkbutton3 = Gtk::make_managed<Gtk::CheckButton>(std::to_string(n));

//     checkbutton1->set_active(true);
//     checkbutton2->set_active(true);
//     checkbutton3->set_active(true);

//     game_affinity_flowbox->add(*checkbutton1);
//     workqueue_affinity_flowbox->add(*checkbutton2);
//     wineserver_affinity_flowbox->add(*checkbutton3);
//   }

//   // We assume that all cores are set to the same frequency governor and that the system has at least one core. In
//   this
//   // case reading the core 0 property should be enough

//   auto list_governors =
//   util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors");

//   auto selected_governor = util::read_system_setting("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor")[0];

//   for (auto& value : list_governors) {
//     if (value.empty()) {
//       continue;
//     }

//     frequency_governor->append(value);
//   }

//   frequency_governor->set_active_text(selected_governor);

//   child_runs_first->set_active(
//       static_cast<bool>(std::stoi(util::read_system_setting("/proc/sys/kernel/sched_child_runs_first")[0])));
// }

// Cpu::~Cpu() {
//   util::debug(log_tag + "destroyed");
// }

// auto Cpu::add_to_stack(Gtk::Stack* stack) -> Cpu* {
//   auto builder = Gtk::Builder::create_from_resource("/com/github/wwmm/fastgame/ui/cpu.glade");

//   Cpu* ui = nullptr;

//   builder->get_widget_derived("widgets_grid", ui);

//   stack->add(*ui, "cpu", _("CPU"));

//   return ui;
// }

// auto Cpu::get_enable_batch_scheduler() -> bool {
//   return use_sched_batch->get_active();
// }

// void Cpu::set_enable_batch_scheduler(const bool& state) {
//   use_sched_batch->set_active(state);
// }

// auto Cpu::get_child_runs_first() -> bool {
//   return child_runs_first->get_active();
// }

// void Cpu::set_child_runs_first(const bool& state) {
//   child_runs_first->set_active(state);
// }

// auto Cpu::get_frequency_governor() -> std::string {
//   return frequency_governor->get_active_text();
// }

// void Cpu::set_frequency_governor(const std::string& name) {
//   frequency_governor->set_active_text(name);
// }

// auto Cpu::get_cores(Gtk::FlowBox* flowbox) -> std::vector<std::string> {
//   std::vector<std::string> list;

//   auto children = flowbox->get_children();

//   for (auto* child : children) {
//     auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

//     auto* checkbutton = dynamic_cast<Gtk::CheckButton*>(flowbox_child->get_child());

//     if (checkbutton->get_active()) {
//       list.emplace_back(checkbutton->get_label());
//     }
//   }

//   return list;
// }

// void Cpu::set_cores(Gtk::FlowBox* flowbox, const std::vector<std::string>& list) {
//   auto children = flowbox->get_children();

//   for (auto* child : children) {
//     auto* flowbox_child = dynamic_cast<Gtk::FlowBoxChild*>(child);

//     auto* checkbutton = dynamic_cast<Gtk::CheckButton*>(flowbox_child->get_child());

//     if (std::find(list.begin(), list.end(), checkbutton->get_label()) != list.end()) {
//       checkbutton->set_active(true);
//     } else {
//       checkbutton->set_active(false);
//     }
//   }
// }

// auto Cpu::get_game_cores() -> std::vector<std::string> {
//   return get_cores(game_affinity_flowbox);
// }

// void Cpu::set_game_cores(const std::vector<std::string>& list) {
//   set_cores(game_affinity_flowbox, list);
// }

// auto Cpu::get_workqueue_cores() -> std::vector<std::string> {
//   return get_cores(workqueue_affinity_flowbox);
// }

// void Cpu::set_workqueue_cores(const std::vector<std::string>& list) {
//   set_cores(workqueue_affinity_flowbox, list);
// }

// auto Cpu::get_wineserver_cores() -> std::vector<std::string> {
//   return get_cores(wineserver_affinity_flowbox);
// }

// void Cpu::set_wineserver_cores(const std::vector<std::string>& list) {
//   set_cores(wineserver_affinity_flowbox, list);
// }

// auto Cpu::get_use_cpu_dma_latency() -> bool {
//   return use_cpu_dma_latency->get_active();
// }

// void Cpu::set_use_cpu_dma_latency(const bool& state) {
//   use_cpu_dma_latency->set_active(state);
// }

// auto Cpu::get_use_realtime_wineserver() -> bool {
//   return realtime_wineserver->get_active();
// }

// void Cpu::set_use_realtime_wineserver(const bool& state) {
//   realtime_wineserver->set_active(state);
// }

// auto Cpu::get_niceness() -> int {
//   return static_cast<int>(niceness->get_value());
// }

// void Cpu::set_niceness(const int& value) {
//   niceness->set_value(value);
// }